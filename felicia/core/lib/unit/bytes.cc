// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

Bytes::Bytes() = default;
Bytes::Bytes(int64_t bytes) : bytes_(bytes) {}

bool Bytes::operator==(Bytes other) const { return bytes_ == other.bytes_; }
bool Bytes::operator!=(Bytes other) const { return bytes_ != other.bytes_; }
bool Bytes::operator<(Bytes other) const { return bytes_ < other.bytes_; }
bool Bytes::operator<=(Bytes other) const { return bytes_ <= other.bytes_; }
bool Bytes::operator>(Bytes other) const { return bytes_ > other.bytes_; }
bool Bytes::operator>=(Bytes other) const { return bytes_ >= other.bytes_; }

Bytes Bytes::operator+(Bytes other) const {
  return Bytes(internal::SaturateAdd(bytes_, other.bytes_));
}
Bytes Bytes::operator-(Bytes other) const {
  return Bytes(internal::SaturateSub(bytes_, other.bytes_));
}
Bytes& Bytes::operator+=(Bytes other) { return *this = (*this + other); }
Bytes& Bytes::operator-=(Bytes other) { return *this = (*this - other); }

double Bytes::operator/(Bytes a) const { return bytes_ / a.bytes_; }

int64_t Bytes::bytes() const { return bytes_; }

// static
Bytes Bytes::FromBytes(int64_t bytes) { return Bytes(bytes); }

// static
Bytes Bytes::FromKilloBytes(int64_t killo_bytes) {
  return Bytes(internal::FromProduct(killo_bytes, kKilloBytes));
}

// static
Bytes Bytes::FromKilloBytesD(double killo_bytes) {
  return FromDouble(killo_bytes * kKilloBytes);
}

// static
Bytes Bytes::FromMegaBytes(int64_t mega_bytes) {
  return Bytes(internal::FromProduct(mega_bytes, kMegaBytes));
}

// static
Bytes Bytes::FromMegaBytesD(double mega_bytes) {
  return FromDouble(mega_bytes * kMegaBytes);
}

// static
Bytes Bytes::FromGigaBytes(int64_t giga_bytes) {
  return Bytes(internal::FromProduct(giga_bytes, kGigaBytes));
}

// static
Bytes Bytes::FromGigaBytesD(double giga_bytes) {
  return FromDouble(giga_bytes * kGigaBytes);
}

// static
Bytes Bytes::Max() { return Bytes(std::numeric_limits<int64_t>::max()); }

// static
Bytes Bytes::Min() { return Bytes(std::numeric_limits<int64_t>::min()); }

// static
Bytes Bytes::FromDouble(double value) {
  return Bytes(base::saturated_cast<int64_t>(value));
}

std::ostream& operator<<(std::ostream& os, Bytes bytes) {
  os << base::NumberToString(bytes.bytes()) << " bytes";
  return os;
}

}  // namespace felicia