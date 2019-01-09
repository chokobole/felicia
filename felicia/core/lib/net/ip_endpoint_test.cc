// Copyright (c) 2012 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#include "felicia/core/lib/net/ip_endpoint.h"

#include "absl/strings/str_cat.h"
#include "gtest/gtest.h"

#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/net/sockaddr_storage.h"

namespace felicia {
namespace net {

namespace {

// Retuns the port field of the |sockaddr|.
const uint16_t* GetPortFieldFromSockaddr(const struct sockaddr* address,
                                         socklen_t address_len) {
  if (address->sa_family == AF_INET) {
    DCHECK_LE(sizeof(sockaddr_in), static_cast<size_t>(address_len));
    const struct sockaddr_in* sockaddr =
        reinterpret_cast<const struct sockaddr_in*>(address);
    return &sockaddr->sin_port;
  } else if (address->sa_family == AF_INET6) {
    DCHECK_LE(sizeof(sockaddr_in6), static_cast<size_t>(address_len));
    const struct sockaddr_in6* sockaddr =
        reinterpret_cast<const struct sockaddr_in6*>(address);
    return &sockaddr->sin6_port;
  } else {
    NOTREACHED();
    return nullptr;
  }
}

// Returns the value of port in |sockaddr| (in host byte ordering).
int GetPortFromSockaddr(const struct sockaddr* address, socklen_t address_len) {
  const uint16_t* port_field = GetPortFieldFromSockaddr(address, address_len);
  if (!port_field) return -1;
  return ntohs(*port_field);
}

struct TestData {
  std::string host;
  std::string host_normalized;
  bool ipv6;
  IPAddress ip_address;
} tests[] = {
    {"127.0.00.1", "127.0.0.1", false},
    {"192.168.1.1", "192.168.1.1", false},
    {"::1", "[::1]", true},
    {"2001:db8:0::42", "[2001:db8::42]", true},
};

class IPEndPointTest : public testing::Test {
 public:
  void SetUp() override {
    // This is where we populate the TestData.
    for (auto& test : tests) {
      EXPECT_TRUE(test.ip_address.AssignFromIPLiteral(test.host));
    }
  }
};

/*
TODO(chokobole): Should enable tests.
TEST_F(IPEndPointTest, Constructor) {
  IPEndPoint endpoint;
  EXPECT_EQ(0, endpoint.port());

  for (const auto& test : tests) {
    IPEndPoint endpoint(test.ip_address, 80);
    EXPECT_EQ(80, endpoint.port());
    EXPECT_EQ(test.ip_address, endpoint.address());
  }
}

TEST_F(IPEndPointTest, Assignment) {
  uint16_t port = 0;
  for (const auto& test : tests) {
    IPEndPoint src(test.ip_address, ++port);
    IPEndPoint dest = src;

    EXPECT_EQ(src.port(), dest.port());
    EXPECT_EQ(src.address(), dest.address());
  }
}

TEST_F(IPEndPointTest, Copy) {
  uint16_t port = 0;
  for (const auto& test : tests) {
    IPEndPoint src(test.ip_address, ++port);
    IPEndPoint dest(src);

    EXPECT_EQ(src.port(), dest.port());
    EXPECT_EQ(src.address(), dest.address());
  }
}

TEST_F(IPEndPointTest, ToFromSockAddr) {
  uint16_t port = 0;
  for (const auto& test : tests) {
    IPEndPoint ip_endpoint(test.ip_address, ++port);

    // Convert to a sockaddr.
    SockaddrStorage storage;
    EXPECT_TRUE(ip_endpoint.ToSockAddr(storage.addr, &storage.addr_len));

    // Basic verification.
    socklen_t expected_size =
        test.ipv6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
    EXPECT_EQ(expected_size, storage.addr_len);
    EXPECT_EQ(ip_endpoint.port(), GetPortFromSockaddr(storage.addr,
                                                      storage.addr_len));

    // And convert back to an IPEndPoint.
    IPEndPoint ip_endpoint2;
    EXPECT_TRUE(ip_endpoint2.FromSockAddr(storage.addr, storage.addr_len));
    EXPECT_EQ(ip_endpoint.port(), ip_endpoint2.port());
    EXPECT_EQ(ip_endpoint.address(), ip_endpoint2.address());
  }
}

TEST_F(IPEndPointTest, ToSockAddrBufTooSmall) {
  uint16_t port = 0;
  for (const auto& test : tests) {
    IPEndPoint ip_endpoint(test.ip_address, port);

    SockaddrStorage storage;
    storage.addr_len = 3;  // size is too small!
    EXPECT_FALSE(ip_endpoint.ToSockAddr(storage.addr, &storage.addr_len));
  }
}

TEST_F(IPEndPointTest, FromSockAddrBufTooSmall) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  IPEndPoint ip_endpoint;
  struct sockaddr* sockaddr = reinterpret_cast<struct sockaddr*>(&addr);
  EXPECT_FALSE(ip_endpoint.FromSockAddr(sockaddr, sizeof(addr) - 1));
}

TEST_F(IPEndPointTest, Equality) {
  uint16_t port = 0;
  for (const auto& test : tests) {
    IPEndPoint src(test.ip_address, ++port);
    IPEndPoint dest(src);
    EXPECT_TRUE(src == dest);
  }
}

TEST_F(IPEndPointTest, LessThan) {
  // Vary by port.
  IPEndPoint ip_endpoint1(tests[0].ip_address, 100);
  IPEndPoint ip_endpoint2(tests[0].ip_address, 1000);
  EXPECT_TRUE(ip_endpoint1 < ip_endpoint2);
  EXPECT_FALSE(ip_endpoint2 < ip_endpoint1);

  // IPv4 vs IPv6
  ip_endpoint1 = IPEndPoint(tests[0].ip_address, 81);
  ip_endpoint2 = IPEndPoint(tests[2].ip_address, 80);
  EXPECT_TRUE(ip_endpoint1 < ip_endpoint2);
  EXPECT_FALSE(ip_endpoint2 < ip_endpoint1);

  // IPv4 vs IPv4
  ip_endpoint1 = IPEndPoint(tests[0].ip_address, 81);
  ip_endpoint2 = IPEndPoint(tests[1].ip_address, 80);
  EXPECT_TRUE(ip_endpoint1 < ip_endpoint2);
  EXPECT_FALSE(ip_endpoint2 < ip_endpoint1);

  // IPv6 vs IPv6
  ip_endpoint1 = IPEndPoint(tests[2].ip_address, 81);
  ip_endpoint2 = IPEndPoint(tests[3].ip_address, 80);
  EXPECT_TRUE(ip_endpoint1 < ip_endpoint2);
  EXPECT_FALSE(ip_endpoint2 < ip_endpoint1);

  // Compare equivalent endpoints.
  ip_endpoint1 = IPEndPoint(tests[0].ip_address, 80);
  ip_endpoint2 = IPEndPoint(tests[0].ip_address, 80);
  EXPECT_FALSE(ip_endpoint1 < ip_endpoint2);
  EXPECT_FALSE(ip_endpoint2 < ip_endpoint1);
}

TEST_F(IPEndPointTest, ToString) {
  IPEndPoint endpoint;
  EXPECT_EQ(0, endpoint.port());

  uint16_t port = 100;
  for (const auto& test : tests) {
    ++port;
    IPEndPoint endpoint(test.ip_address, port);
    const std::string result = endpoint.ToString();
    // TODO(chokobole): convert to_string our version of to_string
    EXPECT_EQ(absl::StrCat(test.host_normalized,":", std::to_string(port)),
result);
  }

  // ToString() shouldn't crash on invalid addresses.
  IPAddress invalid_address;
  IPEndPoint invalid_endpoint(invalid_address, 8080);
  EXPECT_EQ("", invalid_endpoint.ToString());
  EXPECT_EQ("", invalid_endpoint.address().ToString());
}
*/

}  // namespace

}  // namespace net
}  // namespace felicia
