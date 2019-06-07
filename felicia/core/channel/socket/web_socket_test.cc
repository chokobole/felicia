#include "felicia/core/channel/socket/web_socket.h"

#include "gtest/gtest.h"

namespace felicia {

#define READ_MESSAGE(handler, text)                             \
  handler.buffer_->set_offset(0);                               \
  memcpy(handler.buffer_->data(), text.c_str(), text.length()); \
  handler.buffer_->set_offset(handler.buffer_->offset() + text.length())

TEST(WebSocketHandshakeHandler, BasicNegotiate) {
  WebSocket::HandshakeHandler handler(nullptr, channel::WSSettings());
  handler.buffer_->SetCapacity(1024);

  std::string text = "POST / HTTP/1.1\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_FALSE(handler.Parse());

  text = "GET / HTTP/1.0\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_FALSE(handler.Parse());

  text =
      "GET / HTTP/1.1\r\n"
      "Invalid-Key: 11\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Invalid-Key: 11\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Sec-WebSocket-Version: 13\r\n"
      "Sec-WebSocket-Key: value\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Sec-WebSocket-Version: 13\r\n"
      "Sec-WebSocket-Key: \r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Sec-WebSocket-Version: 13\r\n"
      "Sec-WebSocket-Key: value\r\n"
      "Upgrade: websocket\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Sec-WebSocket-Version: 13\r\n"
      "Connection: Upgrade\r\n"
      "Upgrade: websocket\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_FALSE(handler.Validate());

  text =
      "GET / HTTP/1.1\r\n"
      "Sec-WebSocket-Version: 13\r\n"
      "Sec-WebSocket-Key: value\r\n"
      "Connection: Upgrade\r\n"
      "Upgrade: websocket\r\n\r\n";
  READ_MESSAGE(handler, text);
  EXPECT_TRUE(handler.Parse());
  EXPECT_TRUE(handler.Validate());
}

}  // namespace felicia