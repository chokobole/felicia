#ifndef FELICIA_CORE_CHANNEL_TCP_SERVER_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_SERVER_CHANNEL_H_

#include <memory>
#include <vector>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/cancelable_callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/tcp_channel_base.h"

namespace felicia {

class EXPORT TCPServerChannel : public TCPChannelBase {
 public:
  using AcceptCallback = ::base::RepeatingCallback<void(const Status& s)>;
  using AcceptOnceCallback = ::base::OnceCallback<void(const Status& s)>;

  TCPServerChannel();
  ~TCPServerChannel();

  const std::vector<std::unique_ptr<::net::TCPSocket>>& accepted_sockets()
      const;

  bool IsServer() const override;

  bool IsConnected() const override;

  StatusOr<ChannelSource> Listen();

  void DoAcceptLoop(AcceptCallback callback);

  void AcceptOnce(AcceptOnceCallback callback);

  // Write the |buffer| to the |accepted_sockets_|. If it succeeds to write
  // all the sockets, then callback with Status::OK(), otherwise callback
  // with the |write_result_|, which is recorded at every time finishing
  // write.
  void Write(char* buffer, int size, StatusOnceCallback callback) override;
  // Read from the last |accepted_sockets_|. Currently the read case
  // is only happend communication between master and master proxy.
  // TODO(chokobole): Divide TCPServerChannel to for broadcast use and
  // unicast use. For broadcast use, remove the method below.
  void Read(char* buffer, int size, StatusOnceCallback callback) override;

 private:
  int DoAccept();
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnReadAsync(char* buffer,
                   scoped_refptr<::net::IOBufferWithSize> read_buffer,
                   ::net::TCPSocket* socket, int result);
  void OnRead(::net::TCPSocket* socket, int result);
  void OnWrite(::net::TCPSocket* socket, int result);

  void EraseClosedSockets();

  AcceptCallback accept_callback_;
  AcceptOnceCallback accept_once_callback_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  bool has_closed_sockets_ = false;

  std::unique_ptr<::net::TCPSocket> socket_;
  ::net::IPEndPoint accepted_endpoint_;
  std::unique_ptr<::net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<::net::TCPSocket>> accepted_sockets_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_SERVER_CHANNEL_H_