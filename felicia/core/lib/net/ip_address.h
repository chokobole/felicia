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

#ifndef FELICIA_CORE_LIB_NET_IP_ADDRESS_H_
#define FELICIA_CORE_LIB_NET_IP_ADDRESS_H_

#include <array>

#include "absl/strings/string_view.h"

#include "felicia/core/lib/base/compiler_specific.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/logging.h"

namespace felicia {
namespace net {

// Helper class to represent the sequence of bytes in an IP address.
// A vector<uint8_t> would be simpler but incurs heap allocation, so
// IPAddressBytes uses a fixed size array.
class EXPORT IPAddressBytes {
 public:
  IPAddressBytes();
  IPAddressBytes(const uint8_t* data, size_t data_len);
  IPAddressBytes(const IPAddressBytes& other);
  ~IPAddressBytes();

  // Copies |data_len| elements from |data| into this object.
  void Assign(const uint8_t* data, size_t data_len);

  // Returns the number of elements in the underlying array.
  size_t size() const { return size_; }

  // Sets the size to be |size|. Does not actually change the size
  // of the underlying array or zero-initialize the bytes.
  void Resize(size_t size) {
    DCHECK_LE(size, 16u);
    size_ = static_cast<uint8_t>(size);
  }

  // Returns true if the underlying array is empty.
  bool empty() const { return size_ == 0; }

  // Returns a pointer to the underlying array of bytes.
  const uint8_t* data() const { return bytes_.data(); }
  uint8_t* data() { return bytes_.data(); }

  // Returns a pointer to the first element.
  const uint8_t* begin() const { return data(); }
  uint8_t* begin() { return data(); }

  // Returns a pointer past the last element.
  const uint8_t* end() const { return data() + size_; }
  uint8_t* end() { return data() + size_; }

  // Returns a reference to the last element.
  uint8_t& back() {
    DCHECK(!empty());
    return bytes_[size_ - 1];
  }
  const uint8_t& back() const {
    DCHECK(!empty());
    return bytes_[size_ - 1];
  }

  // Appends |val| to the end and increments the size.
  void push_back(uint8_t val) {
    DCHECK_GT(16, size_);
    bytes_[size_++] = val;
  }

  // Returns a reference to the byte at index |pos|.
  uint8_t& operator[](size_t pos) {
    DCHECK_LT(pos, size_);
    return bytes_[pos];
  }
  const uint8_t& operator[](size_t pos) const {
    DCHECK_LT(pos, size_);
    return bytes_[pos];
  }

  bool operator<(const IPAddressBytes& other) const;
  bool operator!=(const IPAddressBytes& other) const;
  bool operator==(const IPAddressBytes& other) const;

 private:
  // Underlying sequence of bytes
  std::array<uint8_t, 16> bytes_;

  // Number of elements in |bytes_|. Should be either kIPv4AddressSize
  // or kIPv6AddressSize or 0.
  uint8_t size_;
};

class EXPORT IPAddress {
 public:
  enum : size_t { kIPv4AddressSize = 4, kIPv6AddressSize = 16 };

  // Creates a zero-sized, invalid address.
  IPAddress();

  IPAddress(const IPAddress& other);

  // Copies the input address to |ip_address_|.
  explicit IPAddress(const IPAddressBytes& address);

  // Copies the input address to |ip_address_|. The input is expected to be in
  // network byte order.
  template <size_t N>
  IPAddress(const uint8_t (&address)[N]) : IPAddress(address, N) {}

  // Copies the input address to |ip_address_| taking an additional length
  // parameter. The input is expected to be in network byte order.
  IPAddress(const uint8_t* address, size_t address_len);

  // Initializes |ip_address_| from the 4 bX bytes to form an IPv4 address.
  // The bytes are expected to be in network byte order.
  IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);

  // Initializes |ip_address_| from the 16 bX bytes to form an IPv6 address.
  // The bytes are expected to be in network byte order.
  IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
            uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8, uint8_t b9,
            uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14,
            uint8_t b15);

  ~IPAddress();

  // Returns true if the IP has |kIPv4AddressSize| elements.
  bool IsIPv4() const;

  // Returns true if the IP has |kIPv6AddressSize| elements.
  bool IsIPv6() const;

  // Returns true if the IP is either an IPv4 or IPv6 address. This function
  // only checks the address length.
  bool IsValid() const;

  // Returns true if the IP is not in a range reserved by the IANA for
  // local networks. Works with both IPv4 and IPv6 addresses.
  // IPv4-mapped-to-IPv6 addresses are considered publicly routable.
  bool IsPubliclyRoutable() const;

  // Returns true if the IP is "zero" (e.g. the 0.0.0.0 IPv4 address).
  bool IsZero() const;

  // Returns true if |ip_address_| is an IPv4-mapped IPv6 address.
  bool IsIPv4MappedIPv6() const;

  // The size in bytes of |ip_address_|.
  size_t size() const { return ip_address_.size(); }

  // Returns true if the IP is an empty, zero-sized (invalid) address.
  bool empty() const { return ip_address_.empty(); }

  // Returns the canonical string representation of an IP address.
  // For example: "192.168.0.1" or "::1". Returns the empty string when
  // |ip_address_| is invalid.
  std::string ToString() const;

  // Parses an IP address literal (either IPv4 or IPv6) to its numeric value.
  // Returns true on success and fills |ip_address_| with the numeric value.
  //
  // When parsing fails, the original value of |this| will be overwritten such
  // that |this->empty()| and |!this->IsValid()|.
  bool AssignFromIPLiteral(const absl::string_view& ip_literal) MUST_USE_RESULT;

  // Returns the underlying bytes.
  const IPAddressBytes& bytes() const { return ip_address_; };

  // Copies the bytes to a new vector. Generally callers should be using
  // |bytes()| and the IPAddressBytes abstraction. This method is provided as a
  // convenience for call sites that existed prior to the introduction of
  // IPAddressBytes.
  std::vector<uint8_t> CopyBytesToVector() const;

  // Returns an IPAddress instance representing the 127.0.0.1 address.
  static IPAddress IPv4Localhost();

  // Returns an IPAddress instance representing the ::1 address.
  static IPAddress IPv6Localhost();

  // Returns an IPAddress made up of |num_zero_bytes| zeros.
  static IPAddress AllZeros(size_t num_zero_bytes);

  // Returns an IPAddress instance representing the 0.0.0.0 address.
  static IPAddress IPv4AllZeros();

  // Returns an IPAddress instance representing the :: address.
  static IPAddress IPv6AllZeros();

  bool operator==(const IPAddress& that) const;
  bool operator!=(const IPAddress& that) const;
  bool operator<(const IPAddress& that) const;

 private:
  IPAddressBytes ip_address_;

  // This class is copyable and assignable.
};

// Converts an IPv4 address to an IPv4-mapped IPv6 address.
// For example 192.168.0.1 would be converted to ::ffff:192.168.0.1.
EXPORT IPAddress ConvertIPv4ToIPv4MappedIPv6(const IPAddress& address);

// Converts an IPv4-mapped IPv6 address to IPv4 address. Should only be called
// on IPv4-mapped IPv6 addresses.
EXPORT IPAddress ConvertIPv4MappedIPv6ToIPv4(const IPAddress& address);

// Checks whether |address| starts with |prefix|. This provides similar
// functionality as IPAddressMatchesPrefix() but doesn't perform automatic IPv4
// to IPv4MappedIPv6 conversions and only checks against full bytes.
template <size_t N>
bool IPAddressStartsWith(const IPAddress& address, const uint8_t (&prefix)[N]) {
  if (address.size() < N) return false;
  return std::equal(prefix, prefix + N, address.bytes().begin());
}

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_NET_IP_ADDRESS_H_
