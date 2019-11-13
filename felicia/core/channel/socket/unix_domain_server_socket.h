// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SERVER_SOCKET_H_

#include "third_party/chromium/net/socket/socket_posix.h"

#include "felicia/core/channel/socket/stream_socket_broadcaster.h"
#include "felicia/core/channel/socket/uds_endpoint.h"
#include "felicia/core/channel/socket/unix_domain_socket.h"
#include "felicia/core/lib/error/statusor.h"

namespace felicia {

class UnixDomainServerSocket : public UnixDomainSocket {
 public:
  // This was taken and modified from
  // https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/unix_domain_server_socket_posix.h#L30-L40
  // Credentials of a peer process connected to the socket.
  struct Credentials {
#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_FUCHSIA)
    // Linux and Fuchsia provide more information about the connected peer
    // than Windows/OS X. It's useful for permission-based authorization on
    // Android.
    pid_t process_id;
#endif
    uid_t user_id;
    gid_t group_id;
  };

  using AcceptCallback = base::RepeatingCallback<void(Status)>;
  using AcceptOnceInterceptCallback =
      base::OnceCallback<void(StatusOr<std::unique_ptr<net::SocketPosix>>)>;
  using AuthCallback = base::RepeatingCallback<bool(const Credentials&)>;

  UnixDomainServerSocket();
  ~UnixDomainServerSocket();

  const std::vector<std::unique_ptr<StreamSocket>>& accepted_sockets() const;

  StatusOr<ChannelDef> BindAndListen();

  void AcceptLoop(AcceptCallback accept_callback, AuthCallback auth_callback);
  void AcceptOnceIntercept(
      AcceptOnceInterceptCallback accept_once_intercept_callback,
      AuthCallback auth_callback);

  // Socket methods
  bool IsServer() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  // Write the |buffer| to the |accepted_sockets_|. If it succeeds to write
  // all the sockets, then callback with Status::OK(), otherwise callback
  // with the |write_result_|, which is recorded at every time finishing
  // write.
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

 private:
  int DoAccept();
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnWrite(Status s);

  static bool GetPeerCredentials(net::SocketDescriptor socket,
                                 Credentials* credentials);

  AcceptCallback accept_callback_;
  AcceptOnceInterceptCallback accept_once_intercept_callback_;
  AuthCallback auth_callback_;

  std::unique_ptr<net::SocketPosix> accepted_socket_;
  std::vector<std::unique_ptr<StreamSocket>> accepted_sockets_;

  StreamSocketBroadcaster broadcaster_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SERVER_SOCKET_H_