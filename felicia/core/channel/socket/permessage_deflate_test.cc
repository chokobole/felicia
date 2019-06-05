#include "felicia/core/channel/socket/permessage_deflate.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(PermessageDeflate, BasicNegotiate) {
  WebSocketExtension extension;
  std::string response;
  EXPECT_TRUE(extension.Negotiate("permessage-deflate; client_max_window_bits",
                                  &response));
  EXPECT_FALSE(
      extension.Negotiate("permessage-deflate; invalid_param", &response));
  EXPECT_FALSE(extension.Negotiate(
      "permessage-deflate; client_max_window_bits=7", &response));
  EXPECT_FALSE(extension.Negotiate(
      "permessage-deflate; client_max_window_bits=16", &response));
}

TEST(PermessageDeflate, Fallback) {
  WebSocketExtension extension;
  std::string response;
  EXPECT_TRUE(
      extension.Negotiate("permessage-deflate; invalid_param, "
                          "permessage-deflate; client_max_window_bits",
                          &response));
}

}  // namespace felicia