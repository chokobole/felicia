#ifndef FELICIA_CORE_COMMUNICATION_TCP_CONNECTION_H_
#define FELICIA_CORE_COMMUNICATION_TCP_CONNECTION_H_

#include "felicia/core/communication/connection.h"

#include "absl/memory/memory.h"

#include "felicia/core/lib/base/macros.h"
#include "felicia/core/platform/tcp_socket.h"

namespace felicia {

class TCPConnection : public Connection {
 public:
  TCPConnection();
  ~TCPConnection();

  bool IsTCPConnection() const override { return true; }

  int Setup() override;
  int Connect() override;
  void OnAccept(int result);

 private:
  void DoAcceptLoop();
  void HandleAccpetResult(int result);

  void OnConnect(int result);

  std::unique_ptr<net::TCPSocket> socket_;
  std::unique_ptr<net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<net::TCPSocket>> accepted_sockets_;

  DISALLOW_COPY_AND_ASSIGN(TCPConnection);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_TCP_CONNECTION_H_