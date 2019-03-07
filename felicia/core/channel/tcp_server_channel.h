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

class TCPServerChannel : public TCPChannelBase {
 public:
  using AcceptCallback = ::base::RepeatingCallback<void(const Status& s)>;

  TCPServerChannel();
  ~TCPServerChannel();

  bool IsServer() const override { return true; }

  bool IsConnected() const override;

  StatusOr<ChannelSource> Listen();

  void DoAcceptLoop(AcceptCallback callback);

  // Write the |buffer| to the |accepted_sockets_|. If it succeeds to write
  // all the sockets, then callback with Status::OK(), otherwise callback
  // with the |write_result_|, which is recorded at every time finishing
  // write.
  void Write(::net::IOBufferWithSize* buffer, StatusCallback callback) override;
  // Read from the first |accepted_sockets_|. Currently the read case
  // is only happend communication between master and master proxy.
  void Read(::net::IOBufferWithSize* buffer, StatusCallback callback) override;

 private:
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnWrite(int result) override;
  void OnWriteTimeout();

  AcceptCallback accept_callback_;
  ::base::CancelableOnceClosure timeout_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  std::unique_ptr<::net::TCPSocket> socket_;
  ::net::IPEndPoint accepted_endpoint_;
  std::unique_ptr<::net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<::net::TCPSocket>> accepted_sockets_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_SERVER_CHANNEL_H_