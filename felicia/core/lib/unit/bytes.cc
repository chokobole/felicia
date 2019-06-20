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
  return SaturateAdd(bytes_, other.bytes_);
}
Bytes Bytes::operator-(Bytes other) const {
  return SaturateSub(bytes_, other.bytes_);
}
Bytes& Bytes::operator+=(Bytes other) { return *this = (*this + other); }
Bytes& Bytes::operator-=(Bytes other) { return *this = (*this - other); }

double Bytes::operator/(Bytes a) const { return bytes_ / a.bytes_; }

int64_t Bytes::bytes() const { return bytes_; }

// static
Bytes Bytes::FromBytes(int64_t bytes) { return Bytes(bytes); }

// static
Bytes Bytes::FromKilloBytes(int64_t killo_bytes) {
  return FromProduct(killo_bytes, kKilloBytes);
}

// static
Bytes Bytes::FromKilloBytesD(double killo_bytes) {
  return FromDouble(killo_bytes * kKilloBytes);
}

// static
Bytes Bytes::FromMegaBytes(int64_t mega_bytes) {
  return FromProduct(mega_bytes, kMegaBytes);
}

// static
Bytes Bytes::FromMegaBytesD(double mega_bytes) {
  return FromDouble(mega_bytes * kMegaBytes);
}

// static
Bytes Bytes::FromGigaBytes(int64_t giga_bytes) {
  return FromProduct(giga_bytes, kGigaBytes);
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
Bytes Bytes::SaturateAdd(int64_t value, int64_t value2) {
  ::base::CheckedNumeric<int64_t> rv(value);
  rv += value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes::Max();
}

// static
Bytes Bytes::SaturateSub(int64_t value, int64_t value2) {
  ::base::CheckedNumeric<int64_t> rv(value);
  rv -= value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes();
}

// static
Bytes Bytes::FromProduct(int64_t value, int64_t multiplier) {
  DCHECK(multiplier > 0);
  return value > std::numeric_limits<int64_t>::max() / multiplier
             ? Bytes::Max()
             : value < std::numeric_limits<int64_t>::min() / multiplier
                   ? Bytes::Min()
                   : Bytes(value * multiplier);
}

// static
Bytes Bytes::FromDouble(double value) {
  return Bytes(::base::saturated_cast<int64_t>(value));
}

std::ostream& operator<<(std::ostream& os, Bytes bytes) {
  os << ::base::NumberToString(bytes.bytes()) << " bytes";
  return os;
}

}  // namespace felicia