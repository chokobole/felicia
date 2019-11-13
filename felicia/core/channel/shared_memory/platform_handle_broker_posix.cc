// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/shared_memory/platform_handle_broker.h"

#include "third_party/chromium/base/posix/unix_domain_socket.h"

#include "felicia/core/channel/socket/unix_domain_client_socket.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/file_util.h"

namespace felicia {

PlatformHandleBroker::PlatformHandleBroker() = default;
PlatformHandleBroker::~PlatformHandleBroker() = default;

StatusOr<ChannelDef> PlatformHandleBroker::Setup(FillDataCallback callback) {
  DCHECK(!broker_);
  broker_ = std::make_unique<UnixDomainServerSocket>();
  UnixDomainServerSocket* server_socket = broker_->ToUnixDomainServerSocket();
  auto status_or = server_socket->BindAndListen();
  if (status_or.ok()) {
    ChannelDef& channel_def = status_or.ValueOrDie();
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_SHM);
    *channel_def.mutable_shm_endpoint()
         ->mutable_broker_endpoint()
         ->mutable_uds_endpoint() = channel_def.uds_endpoint();
    channel_def.clear_uds_endpoint();
    fill_data_callback_ = callback;
    AcceptLoop();
  }

  return status_or;
}

void PlatformHandleBroker::WaitForBroker(ChannelDef channel_def,
                                         ReceiveDataCallback callback) {
  DCHECK(!broker_);
  DCHECK(!callback.is_null());
  DCHECK(receive_data_callback_.is_null());
  net::UDSEndPoint uds_endpoint;
  Status s = ToNetUDSEndPoint(channel_def.shm_endpoint().broker_endpoint(),
                              &uds_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  receive_data_callback_ = std::move(callback);
  broker_ = std::make_unique<UnixDomainClientSocket>();
  UnixDomainClientSocket* client_socket = broker_->ToUnixDomainClientSocket();
  client_socket->Connect(uds_endpoint,
                         base::BindOnce(&PlatformHandleBroker::OnBrokerConnect,
                                        base::Unretained(this)));
}

void PlatformHandleBroker::OnBrokerConnect(Status s) {
  if (!s.ok()) {
    std::move(receive_data_callback_).Run(std::move(s));
    return;
  }

  UnixDomainClientSocket* client_socket = broker_->ToUnixDomainClientSocket();
  int socket_fd = client_socket->socket_fd();
  if (!SetBlocking(socket_fd, true)) {
    std::move(receive_data_callback_)
        .Run(errors::Unavailable("Failed to SetBlocking"));
    return;
  }

  char buf[kDataLen];
  std::vector<base::ScopedFD> fds;
  ssize_t read =
      base::UnixDomainSocket::RecvMsg(socket_fd, buf, kDataLen, &fds);
  if (read < 0) {
    std::move(receive_data_callback_)
        .Run(errors::Unavailable("Failed to RecvMsg"));
    return;
  }

  Data data;
  data.data = std::string(buf, read);
  data.platform_handle.fd = base::kInvalidFd;
  data.platform_handle.readonly_fd = base::kInvalidFd;
  if (fds.size() > 0) {
    data.platform_handle.fd = fds[0].release();
  }
  if (fds.size() > 1) {
    data.platform_handle.readonly_fd = fds[1].release();
  }

  std::move(receive_data_callback_).Run(data);
}

void PlatformHandleBroker::AcceptLoop() {
  UnixDomainServerSocket* server_socket = broker_->ToUnixDomainServerSocket();
  server_socket->AcceptOnceIntercept(
      base::BindOnce(&PlatformHandleBroker::OnBrokerAccept,
                     base::Unretained(this)),
      base::BindRepeating(&PlatformHandleBroker::OnBrokerAuth,
                          base::Unretained(this)));
}

void PlatformHandleBroker::HandleAccept(
    StatusOr<std::unique_ptr<net::SocketPosix>> status_or) {
  if (!status_or.ok()) {
    LOG(ERROR) << status_or.status();
    return;
  }

  int socket_fd = status_or.ValueOrDie()->socket_fd();
  if (!SetBlocking(socket_fd, true)) {
    PLOG(ERROR) << "Failed to SetBlocking";
    return;
  }

  Data data;
  fill_data_callback_.Run(&data);
  if (data.data.length() > kDataLen) {
    LOG(ERROR) << "Data exceeds " << kDataLen;
    return;
  }

  std::vector<int> fds;
  fds.push_back(data.platform_handle.fd);
  if (data.platform_handle.readonly_fd != base::kInvalidFd)
    fds.push_back(data.platform_handle.readonly_fd);

  if (!base::UnixDomainSocket::SendMsg(socket_fd, data.data.c_str(),
                                       data.data.length(), fds)) {
    PLOG(ERROR) << "Failed to SendMsg";
  }
}

void PlatformHandleBroker::OnBrokerAccept(
    StatusOr<std::unique_ptr<net::SocketPosix>> status_or) {
  HandleAccept(std::move(status_or));
  AcceptLoop();
}

bool PlatformHandleBroker::OnBrokerAuth(
    const UnixDomainServerSocket::Credentials& credentials) {
  return true;
}

}  // namespace felicia