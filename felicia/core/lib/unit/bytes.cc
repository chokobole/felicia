#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

Bytes::Bytes() = default;
Bytes::Bytes(size_t bytes) : bytes_(bytes) {}

// static
Bytes Bytes::Max() { return Bytes(std::numeric_limits<size_t>::max()); }

// static
Bytes Bytes::Min() { return Bytes(); }

bool Bytes::operator==(Bytes other) const { return bytes_ == other.bytes_; }
bool Bytes::operator!=(Bytes other) const { return bytes_ != other.bytes_; }
bool Bytes::operator<(Bytes other) const { return bytes_ < other.bytes_; }
bool Bytes::operator<=(Bytes other) const { return bytes_ <= other.bytes_; }
bool Bytes::operator>(Bytes other) const { return bytes_ > other.bytes_; }
bool Bytes::operator>=(Bytes other) const { return bytes_ >= other.bytes_; }

Bytes Bytes::operator+(Bytes other) const {
  return SaturateAdd(bytes_, other.bytes_);
}
Bytes Bytes::operator-(Bytes other) const {
  return SaturateSub(bytes_, other.bytes_);
}
Bytes& Bytes::operator+=(Bytes other) { return *this = (*this + other); }
Bytes& Bytes::operator-=(Bytes other) { return *this = (*this - other); }

double Bytes::operator/(Bytes a) const { return bytes_ / a.bytes_; }

size_t Bytes::bytes() const { return bytes_; }

// static
Bytes Bytes::FromBytes(size_t bytes) { return Bytes(bytes); }

// static
Bytes Bytes::FromKilloBytes(size_t killo_bytes) {
  return FromProduct(killo_bytes, kKilloBytes);
}

// static
Bytes Bytes::FromKilloBytesD(double killo_bytes) {
  return FromDouble(killo_bytes * kKilloBytes);
}

// static
Bytes Bytes::FromMegaBytes(size_t mega_bytes) {
  return FromProduct(mega_bytes, kMegaBytes);
}

// static
Bytes Bytes::FromMegaBytesD(double mega_bytes) {
  return FromDouble(mega_bytes * kMegaBytes);
}

// static
Bytes Bytes::FromGigaBytes(size_t giga_bytes) {
  return FromProduct(giga_bytes, kGigaBytes);
}

// static
Bytes Bytes::FromGigaBytesD(double giga_bytes) {
  return FromDouble(giga_bytes * kGigaBytes);
}

// static
Bytes Bytes::SaturateAdd(size_t value, size_t value2) {
  ::base::CheckedNumeric<size_t> rv(value);
  rv += value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes::Max();
}

// static
Bytes Bytes::SaturateSub(size_t value, size_t value2) {
  ::base::CheckedNumeric<size_t> rv(value);
  rv -= value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes();
}

// static
Bytes Bytes::FromProduct(size_t value, size_t multiplier) {
  DCHECK(multiplier > 0);
  return value > std::numeric_limits<size_t>::max() / multiplier
             ? Bytes::Max()
             : value < std::numeric_limits<size_t>::min() / multiplier
                   ? Bytes::Min()
                   : Bytes(value * multiplier);
}

// static
Bytes Bytes::FromDouble(double value) {
  return Bytes(::base::saturated_cast<size_t>(value));
}

std::ostream& operator<<(std::ostream& os, Bytes bytes) {
  os << ::base::NumberToString(bytes.bytes()) << " bytes";
  return os;
}

}  // namespace felicia