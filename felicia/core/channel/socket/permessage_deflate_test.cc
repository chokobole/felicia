#include "felicia/core/channel/socket/permessage_deflate.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(PermessageDeflate, BasicNegotiate) {
  WebSocketExtension extension;
  channel::WSSettings settings;
  settings.permessage_deflate_enabled = true;
  std::string response;
  EXPECT_TRUE(extension.Negotiate("permessage-deflate; client_max_window_bits",
                                  settings, &response));
  EXPECT_FALSE(extension.Negotiate("permessage-deflate; invalid_param",
                                   settings, &response));
  EXPECT_FALSE(extension.Negotiate(
      "permessage-deflate; client_max_window_bits=7", settings, &response));
  EXPECT_FALSE(extension.Negotiate(
      "permessage-deflate; client_max_window_bits=16", settings, &response));
}

TEST(PermessageDeflate, Fallback) {
  WebSocketExtension extension;
  channel::WSSettings settings;
  settings.permessage_deflate_enabled = true;
  std::string response;
  EXPECT_TRUE(
      extension.Negotiate("permessage-deflate; invalid_param, "
                          "permessage-deflate; client_max_window_bits",
                          settings, &response));
}

}  // namespace felicia