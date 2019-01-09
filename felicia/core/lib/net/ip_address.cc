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

#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/stl_util.h"
#include "felicia/core/lib/net/ip_address.h"

namespace felicia {
namespace net {
namespace {

// The prefix for IPv6 mapped IPv4 addresses.
// https://tools.ietf.org/html/rfc4291#section-2.5.5.2
constexpr uint8_t kIPv4MappedPrefix[] = {0, 0, 0, 0, 0,    0,
                                         0, 0, 0, 0, 0xFF, 0xFF};

// Note that this function assumes:
// * |ip_address| is at least |prefix_length_in_bits| (bits) long;
// * |ip_prefix| is at least |prefix_length_in_bits| (bits) long.
bool IPAddressPrefixCheck(const IPAddressBytes& ip_address,
                          const uint8_t* ip_prefix,
                          size_t prefix_length_in_bits) {
  // Compare all the bytes that fall entirely within the prefix.
  size_t num_entire_bytes_in_prefix = prefix_length_in_bits / 8;
  for (size_t i = 0; i < num_entire_bytes_in_prefix; ++i) {
    if (ip_address[i] != ip_prefix[i]) return false;
  }

  // In case the prefix was not a multiple of 8, there will be 1 byte
  // which is only partially masked.
  size_t remaining_bits = prefix_length_in_bits % 8;
  if (remaining_bits != 0) {
    uint8_t mask = 0xFF << (8 - remaining_bits);
    size_t i = num_entire_bytes_in_prefix;
    if ((ip_address[i] & mask) != (ip_prefix[i] & mask)) return false;
  }
  return true;
}

// Returns false if |ip_address| matches any of the reserved IPv4 ranges. This
// method operates on a blacklist of reserved IPv4 ranges. Some ranges are
// consolidated.
// Sources for info:
// www.iana.org/assignments/ipv4-address-space/ipv4-address-space.xhtml
// www.iana.org/assignments/iana-ipv4-special-registry/
// iana-ipv4-special-registry.xhtml
bool IsPubliclyRoutableIPv4(const IPAddressBytes& ip_address) {
  // Different IP versions have different range reservations.
  DCHECK_EQ(IPAddress::kIPv4AddressSize, ip_address.size());
  struct {
    const uint8_t address[4];
    size_t prefix_length_in_bits;
  } static const kReservedIPv4Ranges[] = {
      {{0, 0, 0, 0}, 8},     {{10, 0, 0, 0}, 8},      {{100, 64, 0, 0}, 10},
      {{127, 0, 0, 0}, 8},   {{169, 254, 0, 0}, 16},  {{172, 16, 0, 0}, 12},
      {{192, 0, 2, 0}, 24},  {{192, 88, 99, 0}, 24},  {{192, 168, 0, 0}, 16},
      {{198, 18, 0, 0}, 15}, {{198, 51, 100, 0}, 24}, {{203, 0, 113, 0}, 24},
      {{224, 0, 0, 0}, 3}};

  for (const auto& range : kReservedIPv4Ranges) {
    if (IPAddressPrefixCheck(ip_address, range.address,
                             range.prefix_length_in_bits)) {
      return false;
    }
  }

  return true;
}

// Returns false if |ip_address| matches any of the IPv6 ranges IANA reserved
// for local networks. This method operates on a whitelist of non-reserved
// IPv6 ranges, plus the blacklist of reserved IPv4 ranges mapped to IPv6.
// Sources for info:
// www.iana.org/assignments/ipv6-address-space/ipv6-address-space.xhtml
bool IsPubliclyRoutableIPv6(const IPAddressBytes& ip_address) {
  DCHECK_EQ(IPAddress::kIPv6AddressSize, ip_address.size());
  struct {
    const uint8_t address_prefix[2];
    size_t prefix_length_in_bits;
  } static const kPublicIPv6Ranges[] = {// 2000::/3  -- Global Unicast
                                        {{0x20, 0}, 3},
                                        // ff00::/8  -- Multicast
                                        {{0xff, 0}, 8}};

  for (const auto& range : kPublicIPv6Ranges) {
    if (IPAddressPrefixCheck(ip_address, range.address_prefix,
                             range.prefix_length_in_bits)) {
      return true;
    }
  }

  IPAddress addr(ip_address);
  if (addr.IsIPv4MappedIPv6()) {
    IPAddress ipv4 = ConvertIPv4MappedIPv6ToIPv4(addr);
    return IsPubliclyRoutableIPv4(ipv4.bytes());
  }

  return false;
}

bool ParseIPLiteralToBytes(const absl::string_view& ip_literal,
                           IPAddressBytes* bytes) {
  NOTIMPLEMENTED();
  return false;
}

std::string IPv4AddressToString(const unsigned char address[4]) {
  // TODO(chokobole): convert to_string our version of to_string
  return absl::StrJoin({std::to_string(address[0]), std::to_string(address[1]),
                        std::to_string(address[2]), std::to_string(address[3])},
                       ".");
}

std::string IPv6AddressToString(const unsigned char address[16]) {
  NOTIMPLEMENTED();
  return std::string();
}

}  // namespace

IPAddressBytes::IPAddressBytes() : size_(0) {}

IPAddressBytes::IPAddressBytes(const uint8_t* data, size_t data_len) {
  Assign(data, data_len);
}

IPAddressBytes::~IPAddressBytes() = default;
IPAddressBytes::IPAddressBytes(const IPAddressBytes& other) = default;

void IPAddressBytes::Assign(const uint8_t* data, size_t data_len) {
  size_ = data_len;
  CHECK_GE(16u, data_len);
  std::copy_n(data, data_len, bytes_.data());
}

bool IPAddressBytes::operator<(const IPAddressBytes& other) const {
  if (size_ == other.size_)
    return std::lexicographical_compare(begin(), end(), other.begin(),
                                        other.end());
  return size_ < other.size_;
}

bool IPAddressBytes::operator==(const IPAddressBytes& other) const {
  return size_ == other.size_ && std::equal(begin(), end(), other.begin());
}

bool IPAddressBytes::operator!=(const IPAddressBytes& other) const {
  return !(*this == other);
}

IPAddress::IPAddress() = default;

IPAddress::IPAddress(const IPAddress& other) = default;

IPAddress::IPAddress(const IPAddressBytes& address) : ip_address_(address) {}

IPAddress::IPAddress(const uint8_t* address, size_t address_len)
    : ip_address_(address, address_len) {}

IPAddress::IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
  ip_address_.push_back(b0);
  ip_address_.push_back(b1);
  ip_address_.push_back(b2);
  ip_address_.push_back(b3);
}

IPAddress::IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
                     uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8, uint8_t b9,
                     uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13,
                     uint8_t b14, uint8_t b15) {
  ip_address_.push_back(b0);
  ip_address_.push_back(b1);
  ip_address_.push_back(b2);
  ip_address_.push_back(b3);
  ip_address_.push_back(b4);
  ip_address_.push_back(b5);
  ip_address_.push_back(b6);
  ip_address_.push_back(b7);
  ip_address_.push_back(b8);
  ip_address_.push_back(b9);
  ip_address_.push_back(b10);
  ip_address_.push_back(b11);
  ip_address_.push_back(b12);
  ip_address_.push_back(b13);
  ip_address_.push_back(b14);
  ip_address_.push_back(b15);
}

IPAddress::~IPAddress() = default;

bool IPAddress::IsIPv4() const {
  return ip_address_.size() == kIPv4AddressSize;
}

bool IPAddress::IsIPv6() const {
  return ip_address_.size() == kIPv6AddressSize;
}

bool IPAddress::IsValid() const { return IsIPv4() || IsIPv6(); }

bool IPAddress::IsPubliclyRoutable() const {
  if (IsIPv4()) {
    return IsPubliclyRoutableIPv4(ip_address_);
  } else if (IsIPv6()) {
    return IsPubliclyRoutableIPv6(ip_address_);
  }
  NOTREACHED();
  return true;
}

bool IPAddress::IsZero() const {
  for (auto x : ip_address_) {
    if (x != 0) return false;
  }

  return !empty();
}

bool IPAddress::IsIPv4MappedIPv6() const {
  return IsIPv6() && IPAddressStartsWith(*this, kIPv4MappedPrefix);
}

bool IPAddress::AssignFromIPLiteral(const absl::string_view& ip_literal) {
  bool success = ParseIPLiteralToBytes(ip_literal, &ip_address_);
  if (!success) ip_address_.Resize(0);
  return success;
}

std::vector<uint8_t> IPAddress::CopyBytesToVector() const {
  return std::vector<uint8_t>(ip_address_.begin(), ip_address_.end());
}

// static
IPAddress IPAddress::IPv4Localhost() {
  static const uint8_t kLocalhostIPv4[] = {127, 0, 0, 1};
  return IPAddress(kLocalhostIPv4);
}

// static
IPAddress IPAddress::IPv6Localhost() {
  static const uint8_t kLocalhostIPv6[] = {0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 1};
  return IPAddress(kLocalhostIPv6);
}

// static
IPAddress IPAddress::AllZeros(size_t num_zero_bytes) {
  CHECK_LE(num_zero_bytes, 16u);
  IPAddress result;
  for (size_t i = 0; i < num_zero_bytes; ++i) result.ip_address_.push_back(0u);
  return result;
}

// static
IPAddress IPAddress::IPv4AllZeros() { return AllZeros(kIPv4AddressSize); }

// static
IPAddress IPAddress::IPv6AllZeros() { return AllZeros(kIPv6AddressSize); }

bool IPAddress::operator==(const IPAddress& that) const {
  return ip_address_ == that.ip_address_;
}

bool IPAddress::operator!=(const IPAddress& that) const {
  return ip_address_ != that.ip_address_;
}

bool IPAddress::operator<(const IPAddress& that) const {
  // Sort IPv4 before IPv6.
  if (ip_address_.size() != that.ip_address_.size()) {
    return ip_address_.size() < that.ip_address_.size();
  }

  return ip_address_ < that.ip_address_;
}

std::string IPAddress::ToString() const {
  if (IsIPv4()) {
    return IPv4AddressToString(ip_address_.data());
  } else if (IsIPv6()) {
    return IPv6AddressToString(ip_address_.data());
  }
  NOTREACHED();
}

IPAddress ConvertIPv4ToIPv4MappedIPv6(const IPAddress& address) {
  DCHECK(address.IsIPv4());
  // IPv4-mapped addresses are formed by:
  // <80 bits of zeros>  + <16 bits of ones> + <32-bit IPv4 address>.
  // TODO(chokobole): Has to allocate memory in stack not on heap.
  std::vector<uint8_t> bytes;
  bytes.insert(bytes.end(), std::begin(kIPv4MappedPrefix),
               std::end(kIPv4MappedPrefix));
  bytes.insert(bytes.end(), address.bytes().begin(), address.bytes().end());
  return IPAddress(bytes.data(), bytes.size());
}

IPAddress ConvertIPv4MappedIPv6ToIPv4(const IPAddress& address) {
  DCHECK(address.IsIPv4MappedIPv6());

  // TODO(chokobole): Has to allocate memory in stack not on heap.
  std::vector<uint8_t> bytes;
  bytes.insert(bytes.end(),
               address.bytes().begin() + felicia::size(kIPv4MappedPrefix),
               address.bytes().end());
  return IPAddress(bytes.data(), bytes.size());
}

}  // namespace net
}  // namespace felicia
