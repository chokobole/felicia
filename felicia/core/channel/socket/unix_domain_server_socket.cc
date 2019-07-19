#include "felicia/core/channel/socket/unix_domain_server_socket.h"

#include "third_party/chromium/base/files/file_util.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/channel/socket/unix_domain_client_socket.h"
#include "felicia/core/lib/error/errors.h"

namespace felicia {

UnixDomainServerSocket::UnixDomainServerSocket()
    : broadcaster_(&accepted_sockets_) {}
UnixDomainServerSocket::~UnixDomainServerSocket() = default;

const std::vector<std::unique_ptr<StreamSocket>>&
UnixDomainServerSocket::accepted_sockets() const {
  return accepted_sockets_;
}

StatusOr<ChannelDef> UnixDomainServerSocket::BindAndListen() {
  auto server_socket = std::make_unique<::net::SocketPosix>();
  int rv = server_socket->Open(AF_UNIX);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  ::net::UDSEndPoint uds_endpoint;
  for (int i = 0; i < 10; ++i) {
    uint64_t random_number =
        ::base::RandGenerator(std::numeric_limits<uint64_t>::max());
    ::base::FilePath socket_path(
        ::base::StrCat({"felicia", ::base::NumberToString(random_number)}));

#if defined(OS_LINUX) || defined(OS_ANDROID)
    uds_endpoint.set_socket_path(socket_path.AsUTF8Unsafe());
    uds_endpoint.set_use_abstract_namespace(true);
#else
    ::base::FilePath path;
    if (!(::base::GetTempDir(&path))) {
      return errors::Aborted("Failed to get temp dir.");
    }
    path = path.Append(socket_path);
    uds_endpoint.set_socket_path(path.AsUTF8Unsafe());
#endif

    ::net::SockaddrStorage address;
    if (!uds_endpoint.ToSockAddrStorage(&address)) {
      return errors::NetworkError(
          ::net::ErrorToString(::net::ERR_ADDRESS_INVALID));
    }

    rv = server_socket->Bind(address);
    if (rv == ::net::OK) break;
  }

  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  rv = server_socket->Listen(5);
  if (rv != ::net::OK) {
    return errors::NetworkError(::net::ErrorToString(rv));
  }

  socket_ = std::move(server_socket);

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_UDS);
  UDSEndPoint* endpoint = channel_def.mutable_uds_endpoint();
  endpoint->set_socket_path(uds_endpoint.socket_path());
  endpoint->set_use_abstract_namespace(uds_endpoint.use_abstract_namespace());
  return channel_def;
}

void UnixDomainServerSocket::AcceptLoop(AcceptCallback accept_callback,
                                        AuthCallback auth_callback) {
  DCHECK(!accept_callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_callback_ = accept_callback;
  auth_callback_ = auth_callback;
  DoAcceptLoop();
}

void UnixDomainServerSocket::AcceptOnceIntercept(
    AcceptOnceInterceptCallback accept_once_intercept_callback,
    AuthCallback auth_callback) {
  DCHECK(!accept_once_intercept_callback.is_null());
  DCHECK(accept_callback_.is_null() &&
         accept_once_intercept_callback_.is_null());
  accept_once_intercept_callback_ = std::move(accept_once_intercept_callback);
  auth_callback_ = auth_callback;
  DoAccept();
}

void UnixDomainServerSocket::AddSocket(
    std::unique_ptr<::net::SocketPosix> socket) {
  auto client_socket = std::make_unique<UnixDomainClientSocket>();
  client_socket->set_socket(std::move(socket));
  accepted_sockets_.push_back(std::move(client_socket));
}

bool UnixDomainServerSocket::IsServer() const { return true; }

bool UnixDomainServerSocket::IsConnected() const {
  for (auto& accepted_socket : accepted_sockets_) {
    if (accepted_socket->IsConnected()) return true;
  }
  return false;
}

void UnixDomainServerSocket::Write(scoped_refptr<::net::IOBuffer> buffer,
                                   int size, StatusOnceCallback callback) {
  DCHECK(write_callback_.is_null());
  write_callback_ = std::move(callback);
  broadcaster_.Broadcast(buffer, size,
                         ::base::BindOnce(&UnixDomainServerSocket::OnWrite,
                                          ::base::Unretained(this)));
}

void UnixDomainServerSocket::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                                  int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket";
}

int UnixDomainServerSocket::DoAccept() {
  int result = socket_->Accept(
      &accepted_socket_, ::base::BindOnce(&UnixDomainServerSocket::OnAccept,
                                          ::base::Unretained(this)));
  if (result != ::net::ERR_IO_PENDING) HandleAccpetResult(result);
  return result;
}

void UnixDomainServerSocket::DoAcceptLoop() {
  int result = ::net::OK;
  while (result == ::net::OK) {
    result = DoAccept();
  }
}

void UnixDomainServerSocket::HandleAccpetResult(int result) {
  DCHECK_NE(result, ::net::ERR_IO_PENDING);

  if (result >= 0 && !auth_callback_.is_null()) {
    Credentials credentials;
    if (!GetPeerCredentials(accepted_socket_->socket_fd(), &credentials) ||
        !auth_callback_.Run(credentials)) {
      accepted_socket_.reset();
      result = ::net::ERR_INVALID_AUTH_CREDENTIALS;
    }
  }

  if (result < 0) {
    if (accept_once_intercept_callback_) {
      std::move(accept_once_intercept_callback_)
          .Run(errors::NetworkError(::net::ErrorToString(result)));
    } else if (accept_callback_) {
      accept_callback_.Run(errors::NetworkError(::net::ErrorToString(result)));
    }
    return;
  }

  if (accept_once_intercept_callback_) {
    std::move(accept_once_intercept_callback_).Run(std::move(accepted_socket_));
  } else {
    auto client_socket = std::make_unique<UnixDomainClientSocket>();
    client_socket->set_socket(std::move(accepted_socket_));
    accepted_sockets_.push_back(std::move(client_socket));
    if (accept_callback_) accept_callback_.Run(Status::OK());
  }
}

void UnixDomainServerSocket::OnAccept(int result) {
  HandleAccpetResult(result);
  if (accept_callback_) DoAcceptLoop();
}

void UnixDomainServerSocket::OnWrite(const Status& s) {
  std::move(write_callback_).Run(s);
}

// This is taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/unix_domain_server_socket_posix.cc#L48-L64
// static
bool UnixDomainServerSocket::GetPeerCredentials(::net::SocketDescriptor socket,
                                                Credentials* credentials) {
#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_FUCHSIA)
  struct ucred user_cred;
  socklen_t len = sizeof(user_cred);
  if (getsockopt(socket, SOL_SOCKET, SO_PEERCRED, &user_cred, &len) < 0)
    return false;
  credentials->process_id = user_cred.pid;
  credentials->user_id = user_cred.uid;
  credentials->group_id = user_cred.gid;
  return true;
#else
  return getpeereid(socket, &credentials->user_id, &credentials->group_id) == 0;
#endif
}

}  // namespace felicia