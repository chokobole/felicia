// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/permessage_deflate.h"

#include "gtest/gtest.h"

namespace felicia {

TEST(PermessageDeflate, BasicNegotiate) {
  WebSocketExtension extension;
  std::vector<WebSocketExtensionInterface*> extensions;
  channel::WSSettings settings;
  settings.permessage_deflate_enabled = true;
  std::string response;
  EXPECT_TRUE(extension.Negotiate("permessage-deflate; client_max_window_bits",
                                  settings, &response, &extensions));
  EXPECT_FALSE(extension.Negotiate("permessage-deflate; invalid_param",
                                   settings, &response, &extensions));
  EXPECT_FALSE(
      extension.Negotiate("permessage-deflate; client_max_window_bits=7",
                          settings, &response, &extensions));
  EXPECT_FALSE(
      extension.Negotiate("permessage-deflate; client_max_window_bits=16",
                          settings, &response, &extensions));
}

TEST(PermessageDeflate, Fallback) {
  WebSocketExtension extension;
  std::vector<WebSocketExtensionInterface*> extensions;
  channel::WSSettings settings;
  settings.permessage_deflate_enabled = true;
  std::string response;
  EXPECT_TRUE(
      extension.Negotiate("permessage-deflate; invalid_param, "
                          "permessage-deflate; client_max_window_bits",
                          settings, &response, &extensions));
}

}  // namespace felicia