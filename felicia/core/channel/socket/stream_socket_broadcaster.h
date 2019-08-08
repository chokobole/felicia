#ifndef FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_BROADCASTER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_BROADCASTER_H_

#include "felicia/core/channel/socket/stream_socket.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class StreamSocketBroadcaster {
 public:
  explicit StreamSocketBroadcaster(
      std::vector<std::unique_ptr<StreamSocket>>* sockets);
  ~StreamSocketBroadcaster();

  void Broadcast(scoped_refptr<net::IOBuffer> buffer, int size,
                 StatusOnceCallback callback);

 private:
  void OnWrite(StreamSocket* socket, int result);

  void EraseClosedSockets();

  StatusOnceCallback callback_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  bool has_closed_sockets_ = false;

  std::vector<std::unique_ptr<StreamSocket>>* sockets_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_BROADCASTER_H_