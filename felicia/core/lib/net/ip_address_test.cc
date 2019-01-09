// Copyright (c) 2015 The Chromium Authors. All rights reserved.
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

#include "felicia/core/lib/net/ip_address.h"

#include "absl/strings/str_format.h"
#include "gtest/gtest.h"

namespace felicia {
namespace net {
namespace {

// Helper to stringize an IP address (used to define expectations).
std::string DumpIPAddress(const IPAddress& v) {
  std::string out;
  for (size_t i = 0; i < v.bytes().size(); ++i) {
    if (i != 0) out.append(",");
    // TODO(chokobole): convert to_string our version of to_string
    out.append(std::to_string(v.bytes()[i]));
  }
  return out;
}

TEST(IPAddressBytesTest, ConstructEmpty) {
  IPAddressBytes bytes;
  ASSERT_EQ(0u, bytes.size());
}

TEST(IPAddressBytesTest, ConstructIPv4) {
  uint8_t data[] = {192, 168, 1, 1};
  IPAddressBytes bytes(data, 4);
  ASSERT_EQ(4, bytes.size());
  size_t i = 0;
  for (uint8_t byte : bytes) EXPECT_EQ(data[i++], byte);
  ASSERT_EQ(4, i);
}

TEST(IPAddressBytesTest, ConstructIPv6) {
  uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  IPAddressBytes bytes(data, 16);
  ASSERT_EQ(16, bytes.size());
  size_t i = 0;
  for (uint8_t byte : bytes) EXPECT_EQ(data[i++], byte);
  ASSERT_EQ(16, i);
}

TEST(IPAddressBytesTest, Assign) {
  uint8_t data[] = {192, 168, 1, 1};
  IPAddressBytes copy;
  copy.Assign(data, 4);
  EXPECT_EQ(IPAddressBytes(data, 4), copy);
}

TEST(IPAddressTest, ConstructIPv4) {
  EXPECT_EQ("127.0.0.1", IPAddress::IPv4Localhost().ToString());

  IPAddress ipv4_ctor(192, 168, 1, 1);
  EXPECT_EQ("192.168.1.1", ipv4_ctor.ToString());
}

TEST(IPAddressTest, IsIPVersion) {
  uint8_t addr1[4] = {192, 168, 0, 1};
  IPAddress ip_address1(addr1);
  EXPECT_TRUE(ip_address1.IsIPv4());
  EXPECT_FALSE(ip_address1.IsIPv6());

  uint8_t addr2[16] = {0xFE, 0xDC, 0xBA, 0x98};
  IPAddress ip_address2(addr2);
  EXPECT_TRUE(ip_address2.IsIPv6());
  EXPECT_FALSE(ip_address2.IsIPv4());

  IPAddress ip_address3;
  EXPECT_FALSE(ip_address3.IsIPv6());
  EXPECT_FALSE(ip_address3.IsIPv4());
}

TEST(IPAddressTest, IsValid) {
  uint8_t addr1[4] = {192, 168, 0, 1};
  IPAddress ip_address1(addr1);
  EXPECT_TRUE(ip_address1.IsValid());
  EXPECT_FALSE(ip_address1.empty());

  uint8_t addr2[16] = {0xFE, 0xDC, 0xBA, 0x98};
  IPAddress ip_address2(addr2);
  EXPECT_TRUE(ip_address2.IsValid());
  EXPECT_FALSE(ip_address2.empty());

  uint8_t addr3[5] = {0xFE, 0xDC, 0xBA, 0x98};
  IPAddress ip_address3(addr3);
  EXPECT_FALSE(ip_address3.IsValid());
  EXPECT_FALSE(ip_address3.empty());

  IPAddress ip_address4;
  EXPECT_FALSE(ip_address4.IsValid());
  EXPECT_TRUE(ip_address4.empty());
}

enum IPAddressReservedResult : bool { NOT_RESERVED = false, RESERVED = true };

/*
TODO(chokobole): Should enable these tests
// Tests for the reserved IPv4 ranges and the (unreserved) blocks in between.
// The reserved ranges are tested by checking the first and last address of each
// range. The unreserved blocks are tested similarly. These tests cover the
// entire IPv4 address range, as well as this range mapped to IPv6.
TEST(IPAddressTest, IsPubliclyRoutableIPv4) {
  struct {
    const char* const address;
    IPAddressReservedResult is_reserved;
  } tests[] = {// 0.0.0.0/8
               {"0.0.0.0", RESERVED},
               {"0.255.255.255", RESERVED},
               // Unreserved block(s)
               {"1.0.0.0", NOT_RESERVED},
               {"9.255.255.255", NOT_RESERVED},
               // 10.0.0.0/8
               {"10.0.0.0", RESERVED},
               {"10.255.255.255", RESERVED},
               // Unreserved block(s)
               {"11.0.0.0", NOT_RESERVED},
               {"100.63.255.255", NOT_RESERVED},
               // 100.64.0.0/10
               {"100.64.0.0", RESERVED},
               {"100.127.255.255", RESERVED},
               // Unreserved block(s)
               {"100.128.0.0", NOT_RESERVED},
               {"126.255.255.255", NOT_RESERVED},
               // 127.0.0.0/8
               {"127.0.0.0", RESERVED},
               {"127.255.255.255", RESERVED},
               // Unreserved block(s)
               {"128.0.0.0", NOT_RESERVED},
               {"169.253.255.255", NOT_RESERVED},
               // 169.254.0.0/16
               {"169.254.0.0", RESERVED},
               {"169.254.255.255", RESERVED},
               // Unreserved block(s)
               {"169.255.0.0", NOT_RESERVED},
               {"172.15.255.255", NOT_RESERVED},
               // 172.16.0.0/12
               {"172.16.0.0", RESERVED},
               {"172.31.255.255", RESERVED},
               // Unreserved block(s)
               {"172.32.0.0", NOT_RESERVED},
               {"191.255.255.255", NOT_RESERVED},
               // 192.0.0.0/24 (including sub ranges)
               {"192.0.0.0", NOT_RESERVED},
               {"192.0.0.255", NOT_RESERVED},
               // Unreserved block(s)
               {"192.0.1.0", NOT_RESERVED},
               {"192.0.1.255", NOT_RESERVED},
               // 192.0.2.0/24
               {"192.0.2.0", RESERVED},
               {"192.0.2.255", RESERVED},
               // Unreserved block(s)
               {"192.0.3.0", NOT_RESERVED},
               {"192.31.195.255", NOT_RESERVED},
               // 192.31.196.0/24
               {"192.31.196.0", NOT_RESERVED},
               {"192.31.196.255", NOT_RESERVED},
               // Unreserved block(s)
               {"192.32.197.0", NOT_RESERVED},
               {"192.52.192.255", NOT_RESERVED},
               // 192.52.193.0/24
               {"192.52.193.0", NOT_RESERVED},
               {"192.52.193.255", NOT_RESERVED},
               // Unreserved block(s)
               {"192.52.194.0", NOT_RESERVED},
               {"192.88.98.255", NOT_RESERVED},
               // 192.88.99.0/24
               {"192.88.99.0", RESERVED},
               {"192.88.99.255", RESERVED},
               // Unreserved block(s)
               {"192.88.100.0", NOT_RESERVED},
               {"192.167.255.255", NOT_RESERVED},
               // 192.168.0.0/16
               {"192.168.0.0", RESERVED},
               {"192.168.255.255", RESERVED},
               // Unreserved block(s)
               {"192.169.0.0", NOT_RESERVED},
               {"192.175.47.255", NOT_RESERVED},
               // 192.175.48.0/24
               {"192.175.48.0", NOT_RESERVED},
               {"192.175.48.255", NOT_RESERVED},
               // Unreserved block(s)
               {"192.175.49.0", NOT_RESERVED},
               {"198.17.255.255", NOT_RESERVED},
               // 198.18.0.0/15
               {"198.18.0.0", RESERVED},
               {"198.19.255.255", RESERVED},
               // Unreserved block(s)
               {"198.20.0.0", NOT_RESERVED},
               {"198.51.99.255", NOT_RESERVED},
               // 198.51.100.0/24
               {"198.51.100.0", RESERVED},
               {"198.51.100.255", RESERVED},
               // Unreserved block(s)
               {"198.51.101.0", NOT_RESERVED},
               {"203.0.112.255", NOT_RESERVED},
               // 203.0.113.0/24
               {"203.0.113.0", RESERVED},
               {"203.0.113.255", RESERVED},
               // Unreserved block(s)
               {"203.0.114.0", NOT_RESERVED},
               {"223.255.255.255", NOT_RESERVED},
               // 224.0.0.0/8 - 255.0.0.0/8
               {"224.0.0.0", RESERVED},
               {"255.255.255.255", RESERVED}};

  IPAddress address;
  IPAddress mapped_address;
  for (const auto& test : tests) {
    EXPECT_TRUE(address.AssignFromIPLiteral(test.address));
    ASSERT_TRUE(address.IsValid());
    EXPECT_EQ(!test.is_reserved, address.IsPubliclyRoutable());

    // Check these IPv4 addresses when mapped to IPv6. This verifies we're
    // properly unpacking mapped addresses.
    IPAddress mapped_address = ConvertIPv4ToIPv4MappedIPv6(address);
    EXPECT_EQ(!test.is_reserved, mapped_address.IsPubliclyRoutable());
  }
}

// Tests for the reserved IPv6 ranges and the (unreserved) blocks in between.
// The reserved ranges are tested by checking the first and last address of each
// range. The unreserved blocks are tested similarly. These tests cover the
// entire IPv6 address range.
TEST(IPAddressTest, IsPubliclyRoutableIPv6) {
  struct {
    const char* const address;
    IPAddressReservedResult is_reserved;
  } tests[] = {// 0000::/8.
               // Skip testing ::ffff:/96 explicitly since it was tested
               // in IsPubliclyRoutableIPv4
               {"0:0:0:0:0:0:0:0", RESERVED},
               {"ff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 0100::/8
               {"100:0:0:0:0:0:0:0", RESERVED},
               {"1ff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 0200::/7
               {"200:0:0:0:0:0:0:0", RESERVED},
               {"3ff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 0400::/6
               {"400:0:0:0:0:0:0:0", RESERVED},
               {"7ff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 0800::/5
               {"800:0:0:0:0:0:0:0", RESERVED},
               {"fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 1000::/4
               {"1000:0:0:0:0:0:0:0", RESERVED},
               {"1fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 2000::/3 (Global Unicast)
               {"2000:0:0:0:0:0:0:0", NOT_RESERVED},
               {"3fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", NOT_RESERVED},
               // 4000::/3
               {"4000:0:0:0:0:0:0:0", RESERVED},
               {"5fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 6000::/3
               {"6000:0:0:0:0:0:0:0", RESERVED},
               {"7fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // 8000::/3
               {"8000:0:0:0:0:0:0:0", RESERVED},
               {"9fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // c000::/3
               {"c000:0:0:0:0:0:0:0", RESERVED},
               {"dfff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // e000::/4
               {"e000:0:0:0:0:0:0:0", RESERVED},
               {"efff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // f000::/5
               {"f000:0:0:0:0:0:0:0", RESERVED},
               {"f7ff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // f800::/6
               {"f800:0:0:0:0:0:0:0", RESERVED},
               {"fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // fc00::/7
               {"fc00:0:0:0:0:0:0:0", RESERVED},
               {"fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // fe00::/9
               {"fe00:0:0:0:0:0:0:0", RESERVED},
               {"fe7f:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // fe80::/10
               {"fe80:0:0:0:0:0:0:0", RESERVED},
               {"febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // fec0::/10
               {"fec0:0:0:0:0:0:0:0", RESERVED},
               {"feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", RESERVED},
               // ff00::/8 (Multicast)
               {"ff00:0:0:0:0:0:0:0", NOT_RESERVED},
               {"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", NOT_RESERVED}};

  IPAddress address;
  for (const auto& test : tests) {
    EXPECT_TRUE(address.AssignFromIPLiteral(test.address));
    EXPECT_EQ(!test.is_reserved, address.IsPubliclyRoutable());
  }
}
*/

TEST(IPAddressTest, IsZero) {
  uint8_t address1[4] = {};
  IPAddress zero_ipv4_address(address1);
  EXPECT_TRUE(zero_ipv4_address.IsZero());

  uint8_t address2[4] = {10};
  IPAddress non_zero_ipv4_address(address2);
  EXPECT_FALSE(non_zero_ipv4_address.IsZero());

  uint8_t address3[16] = {};
  IPAddress zero_ipv6_address(address3);
  EXPECT_TRUE(zero_ipv6_address.IsZero());

  uint8_t address4[16] = {10};
  IPAddress non_zero_ipv6_address(address4);
  EXPECT_FALSE(non_zero_ipv6_address.IsZero());

  IPAddress empty_address;
  EXPECT_FALSE(empty_address.IsZero());
}

TEST(IPAddressTest, IsIPv4Mapped) {
  IPAddress ipv4_address(192, 168, 0, 1);
  EXPECT_FALSE(ipv4_address.IsIPv4MappedIPv6());
  IPAddress ipv6_address(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  EXPECT_FALSE(ipv6_address.IsIPv4MappedIPv6());
  IPAddress mapped_address(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 1, 1, 0, 1);
  EXPECT_TRUE(mapped_address.IsIPv4MappedIPv6());
}

/*
TODO(chokobole): Should enable these tests.
TEST(IPAddressTest, AllZeros) {
  EXPECT_TRUE(IPAddress::AllZeros(0).empty());

  EXPECT_EQ(3u, IPAddress::AllZeros(3).size());
  EXPECT_TRUE(IPAddress::AllZeros(3).IsZero());

  EXPECT_EQ("0.0.0.0", IPAddress::IPv4AllZeros().ToString());
  EXPECT_EQ("::", IPAddress::IPv6AllZeros().ToString());
}

TEST(IPAddressTest, ToString) {
  EXPECT_EQ("0.0.0.0", IPAddress::IPv4AllZeros().ToString());

  IPAddress address(192, 168, 0, 1);
  EXPECT_EQ("192.168.0.1", address.ToString());

  IPAddress address2(0xFE, 0xDC, 0xBA, 0x98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0);
  EXPECT_EQ("fedc:ba98::", address2.ToString());

  // ToString() shouldn't crash on invalid addresses.
  uint8_t addr4[2];
  IPAddress address4(addr4);
  EXPECT_EQ("", address4.ToString());

  IPAddress address5;
  EXPECT_EQ("", address5.ToString());
}

// Test that invalid IP literals fail to parse.
TEST(IPAddressTest, AssignFromIPLiteral_FailParse) {
  IPAddress address;

  EXPECT_FALSE(address.AssignFromIPLiteral("bad value"));
  EXPECT_FALSE(address.AssignFromIPLiteral("bad:value"));
  EXPECT_FALSE(address.AssignFromIPLiteral(std::string()));
  EXPECT_FALSE(address.AssignFromIPLiteral("192.168.0.1:30"));
  EXPECT_FALSE(address.AssignFromIPLiteral("  192.168.0.1  "));
  EXPECT_FALSE(address.AssignFromIPLiteral("[::1]"));
}

// Test that a failure calling AssignFromIPLiteral() has the sideffect of
// clearing the current value.
TEST(IPAddressTest, AssignFromIPLiteral_ResetOnFailure) {
  IPAddress address = IPAddress::IPv6Localhost();

  EXPECT_TRUE(address.IsValid());
  EXPECT_FALSE(address.empty());

  EXPECT_FALSE(address.AssignFromIPLiteral("bad value"));

  EXPECT_FALSE(address.IsValid());
  EXPECT_TRUE(address.empty());
}

// Test parsing an IPv4 literal.
TEST(IPAddressTest, AssignFromIPLiteral_IPv4) {
  IPAddress address;
  EXPECT_TRUE(address.AssignFromIPLiteral("192.168.0.1"));
  EXPECT_EQ("192,168,0,1", DumpIPAddress(address));
  EXPECT_EQ("192.168.0.1", address.ToString());
}

// Test parsing an IPv6 literal.
TEST(IPAddressTest, AssignFromIPLiteral_IPv6) {
  IPAddress address;
  EXPECT_TRUE(address.AssignFromIPLiteral("1:abcd::3:4:ff"));
  EXPECT_EQ("0,1,171,205,0,0,0,0,0,0,0,3,0,4,0,255", DumpIPAddress(address));
  EXPECT_EQ("1:abcd::3:4:ff", address.ToString());
}
*/

TEST(IPAddressTest, IsIPv4MappedIPv6) {
  IPAddress ipv4_address(192, 168, 0, 1);
  EXPECT_FALSE(ipv4_address.IsIPv4MappedIPv6());
  IPAddress ipv6_address = IPAddress::IPv6Localhost();
  EXPECT_FALSE(ipv6_address.IsIPv4MappedIPv6());
  IPAddress mapped_address(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 1, 1, 0, 1);
  EXPECT_TRUE(mapped_address.IsIPv4MappedIPv6());
}

/*
TODO(chokobole): Should enable these tests.
TEST(IPAddressTest, IsEqual) {
  IPAddress ip_address1;
  EXPECT_TRUE(ip_address1.AssignFromIPLiteral("127.0.0.1"));
  IPAddress ip_address2;
  EXPECT_TRUE(ip_address2.AssignFromIPLiteral("2001:db8:0::42"));
  IPAddress ip_address3;
  EXPECT_TRUE(ip_address3.AssignFromIPLiteral("127.0.0.1"));

  EXPECT_FALSE(ip_address1 == ip_address2);
  EXPECT_TRUE(ip_address1 == ip_address3);
}

TEST(IPAddressTest, LessThan) {
  // IPv4 vs IPv6
  IPAddress ip_address1;
  EXPECT_TRUE(ip_address1.AssignFromIPLiteral("127.0.0.1"));
  IPAddress ip_address2;
  EXPECT_TRUE(ip_address2.AssignFromIPLiteral("2001:db8:0::42"));
  EXPECT_TRUE(ip_address1 < ip_address2);
  EXPECT_FALSE(ip_address2 < ip_address1);

  // Compare equivalent addresses.
  IPAddress ip_address3;
  EXPECT_TRUE(ip_address3.AssignFromIPLiteral("127.0.0.1"));
  EXPECT_FALSE(ip_address1 < ip_address3);
  EXPECT_FALSE(ip_address3 < ip_address1);

  IPAddress ip_address4;
  EXPECT_TRUE(ip_address4.AssignFromIPLiteral("128.0.0.0"));
  EXPECT_TRUE(ip_address1 < ip_address4);
  EXPECT_FALSE(ip_address4 < ip_address1);
}

// Test mapping an IPv4 address to an IPv6 address.
TEST(IPAddressTest, ConvertIPv4ToIPv4MappedIPv6) {
  IPAddress ipv4_address(192, 168, 0, 1);
  IPAddress ipv6_address = ConvertIPv4ToIPv4MappedIPv6(ipv4_address);

  // ::ffff:192.168.0.1
  EXPECT_EQ("0,0,0,0,0,0,0,0,0,0,255,255,192,168,0,1",
            DumpIPAddress(ipv6_address));
  EXPECT_EQ("::ffff:c0a8:1", ipv6_address.ToString());
}

// Test reversal of a IPv6 address mapping.
TEST(IPAddressTest, ConvertIPv4MappedIPv6ToIPv4) {
  IPAddress ipv4mapped_address;
  EXPECT_TRUE(ipv4mapped_address.AssignFromIPLiteral("::ffff:c0a8:1"));

  IPAddress expected(192, 168, 0, 1);

  IPAddress result = ConvertIPv4MappedIPv6ToIPv4(ipv4mapped_address);
  EXPECT_EQ(expected, result);
}

TEST(IPAddressTest, IPAddressStartsWith) {
  IPAddress ipv4_address(192, 168, 10, 5);

  uint8_t ipv4_prefix1[] = {192, 168, 10};
  EXPECT_TRUE(IPAddressStartsWith(ipv4_address, ipv4_prefix1));

  uint8_t ipv4_prefix3[] = {192, 168, 10, 5};
  EXPECT_TRUE(IPAddressStartsWith(ipv4_address, ipv4_prefix3));

  uint8_t ipv4_prefix2[] = {192, 168, 10, 10};
  EXPECT_FALSE(IPAddressStartsWith(ipv4_address, ipv4_prefix2));

  // Prefix is longer than the address.
  uint8_t ipv4_prefix4[] = {192, 168, 10, 10, 0};
  EXPECT_FALSE(IPAddressStartsWith(ipv4_address, ipv4_prefix4));

  IPAddress ipv6_address;
  EXPECT_TRUE(ipv6_address.AssignFromIPLiteral("2a00:1450:400c:c09::64"));

  uint8_t ipv6_prefix1[] = {42, 0, 20, 80, 64, 12, 12, 9};
  EXPECT_TRUE(IPAddressStartsWith(ipv6_address, ipv6_prefix1));

  uint8_t ipv6_prefix2[] = {41, 0, 20, 80, 64, 12, 12, 9,
                            0,  0, 0,  0,  0,  0,  100};
  EXPECT_FALSE(IPAddressStartsWith(ipv6_address, ipv6_prefix2));

  uint8_t ipv6_prefix3[] = {42, 0, 20, 80, 64, 12, 12, 9,
                            0,  0, 0,  0,  0,  0,  0,  100};
  EXPECT_TRUE(IPAddressStartsWith(ipv6_address, ipv6_prefix3));

  uint8_t ipv6_prefix4[] = {42, 0, 20, 80, 64, 12, 12, 9,
                            0,  0, 0,  0,  0,  0,  0,  0};
  EXPECT_FALSE(IPAddressStartsWith(ipv6_address, ipv6_prefix4));

  // Prefix is longer than the address.
  uint8_t ipv6_prefix5[] = {42, 0, 20, 80, 64, 12, 12, 9, 0,
                            0,  0, 0,  0,  0,  0,  0,  10};
  EXPECT_FALSE(IPAddressStartsWith(ipv6_address, ipv6_prefix5));
}
*/

}  // namespace
}  // namespace net
}  // namespace felicia