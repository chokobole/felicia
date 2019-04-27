#ifndef FELICIA_CORE_LIB_UNIT_BYTES_H_
#define FELICIA_CORE_LIB_UNIT_BYTES_H_

#include "felicia/core/lib/base/export.h"

#include "third_party/chromium/base/numerics/safe_math.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

namespace felicia {

class EXPORT Bytes {
 public:
  static constexpr size_t kKilloBytes = 1000;
  static constexpr size_t kMegaBytes = 1000 * kKilloBytes;
  static constexpr size_t kGigaBytes = 1000 * kMegaBytes;

  constexpr Bytes() = default;
  constexpr Bytes(size_t bytes) : bytes_(bytes) {}

  static constexpr Bytes FromBytes(size_t bytes);
  static constexpr Bytes FromKilloBytes(size_t killo_bytes);
  static constexpr Bytes FromKilloBytesD(double killo_bytes);
  static constexpr Bytes FromMegaBytes(size_t mega_bytes);
  static constexpr Bytes FromMegaBytesD(double mega_bytes);
  static constexpr Bytes FromGigaBytes(size_t giga_bytes);
  static constexpr Bytes FromGigaBytesD(double giga_bytes);

  static constexpr Bytes Max() {
    return Bytes(std::numeric_limits<size_t>::max());
  }

  static constexpr Bytes Min() { return Bytes(); }

  constexpr bool operator==(Bytes other) const {
    return bytes_ == other.bytes_;
  }
  constexpr bool operator!=(Bytes other) const {
    return bytes_ != other.bytes_;
  }
  constexpr bool operator<(Bytes other) const { return bytes_ < other.bytes_; }
  constexpr bool operator<=(Bytes other) const {
    return bytes_ <= other.bytes_;
  }
  constexpr bool operator>(Bytes other) const { return bytes_ > other.bytes_; }
  constexpr bool operator>=(Bytes other) const {
    return bytes_ >= other.bytes_;
  }

  // See third_party/chromium/base/time/time.h, why didn't put constexpr here.
  Bytes operator+(Bytes other) const {
    return SaturateAdd(bytes_, other.bytes_);
  }
  Bytes operator-(Bytes other) const {
    return SaturateSub(bytes_, other.bytes_);
  }
  Bytes& operator+=(Bytes other) { return *this = (*this + other); }
  Bytes& operator-=(Bytes other) { return *this = (*this - other); }

  // See third_party/chromium/base/time/time.h, why didn't put constexpr here.
  template <typename T>
  Bytes operator*(T a) const {
    ::base::CheckedNumeric<size_t> rv(bytes_);
    rv *= a;
    if (rv.IsValid()) return Bytes(rv.ValueOrDie());
    // Matched sign overflows. Mismatched sign underflows.
    if ((bytes_ < 0) ^ (a < 0)) return Bytes();
    return Bytes::Max();
  }
  template <typename T>
  constexpr Bytes operator/(T a) const {
    ::base::CheckedNumeric<size_t> rv(bytes_);
    rv /= a;
    if (rv.IsValid()) return Bytes(rv.ValueOrDie());
    // Matched sign overflows. Mismatched sign underflows.
    // Special case to catch divide by zero.
    if ((bytes_ < 0) ^ (a <= 0)) return Bytes();
    return Bytes::Max();
  }
  template <typename T>
  Bytes& operator*=(T a) {
    return *this = (*this * a);
  }
  template <typename T>
  constexpr Bytes& operator/=(T a) {
    return *this = (*this / a);
  }

  constexpr double operator/(Bytes a) const { return bytes_ / a.bytes_; }

  size_t bytes() const { return bytes_; }

 private:
  static Bytes SaturateAdd(size_t value, size_t value2);
  static Bytes SaturateSub(size_t value, size_t value2);
  static constexpr Bytes FromProduct(size_t value, size_t positive_value);
  static constexpr Bytes FromDouble(double value);

  size_t bytes_ = 0;
};

template <typename T>
Bytes operator*(T a, Bytes bytes) {
  return bytes * a;
}

// static
constexpr Bytes Bytes::FromBytes(size_t bytes) { return Bytes(bytes); }

// static
constexpr Bytes Bytes::FromKilloBytes(size_t killo_bytes) {
  return FromProduct(killo_bytes, kKilloBytes);
}

// static
constexpr Bytes Bytes::FromKilloBytesD(double killo_bytes) {
  return FromDouble(killo_bytes * kKilloBytes);
}

// static
constexpr Bytes Bytes::FromMegaBytes(size_t mega_bytes) {
  return FromProduct(mega_bytes, kMegaBytes);
}

// static
constexpr Bytes Bytes::FromMegaBytesD(double mega_bytes) {
  return FromDouble(mega_bytes * kMegaBytes);
}

// static
constexpr Bytes Bytes::FromGigaBytes(size_t giga_bytes) {
  return FromProduct(giga_bytes, kGigaBytes);
}

// static
constexpr Bytes Bytes::FromGigaBytesD(double giga_bytes) {
  return FromDouble(giga_bytes * kGigaBytes);
}

// static
constexpr Bytes Bytes::FromProduct(size_t value, size_t multiplier) {
  DCHECK(multiplier > 0);
  return value > std::numeric_limits<size_t>::max() / multiplier
             ? Bytes::Max()
             : value < std::numeric_limits<size_t>::min() / multiplier
                   ? Bytes::Min()
                   : Bytes(value * multiplier);
}

// static
constexpr Bytes Bytes::FromDouble(double value) {
  return Bytes(::base::saturated_cast<size_t>(value));
}

EXPORT std::ostream& operator<<(std::ostream& os, Bytes time);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_BYTE_H_