#ifndef FELICIA_CORE_LIB_UNIT_BYTES_H_
#define FELICIA_CORE_LIB_UNIT_BYTES_H_

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/unit_helper.h"

namespace felicia {

class FEL_EXPORT Bytes {
 public:
  static constexpr int64_t kKilloBytes = 1000;
  static constexpr int64_t kMegaBytes = 1000 * kKilloBytes;
  static constexpr int64_t kGigaBytes = 1000 * kMegaBytes;

  Bytes();
  explicit Bytes(int64_t bytes);

  static Bytes FromBytes(int64_t bytes);
  static Bytes FromKilloBytes(int64_t killo_bytes);
  static Bytes FromKilloBytesD(double killo_bytes);
  static Bytes FromMegaBytes(int64_t mega_bytes);
  static Bytes FromMegaBytesD(double mega_bytes);
  static Bytes FromGigaBytes(int64_t giga_bytes);
  static Bytes FromGigaBytesD(double giga_bytes);

  static Bytes Max();
  static Bytes Min();

  bool operator==(Bytes other) const;
  bool operator!=(Bytes other) const;
  bool operator<(Bytes other) const;
  bool operator<=(Bytes other) const;
  bool operator>(Bytes other) const;
  bool operator>=(Bytes other) const;

  Bytes operator+(Bytes other) const;
  Bytes operator-(Bytes other) const;
  Bytes& operator+=(Bytes other);
  Bytes& operator-=(Bytes other);

  template <typename T>
  Bytes operator*(T a) const {
    return Bytes(internal::SaturateMul(bytes_, a));
  }
  template <typename T>
  Bytes operator/(T a) const {
    return Bytes(internal::SaturateDiv(bytes_, a));
  }
  template <typename T>
  Bytes& operator*=(T a) {
    return *this = (*this * a);
  }
  template <typename T>
  Bytes& operator/=(T a) {
    return *this = (*this / a);
  }

  double operator/(Bytes a) const;

  int64_t bytes() const;

 private:
  static Bytes FromDouble(double value);

  int64_t bytes_ = 0;
};

template <typename T>
Bytes operator*(T a, Bytes bytes) {
  return bytes * a;
}

FEL_EXPORT std::ostream& operator<<(std::ostream& os, Bytes bytes);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_BYTE_H_