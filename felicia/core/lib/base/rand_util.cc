#include "felicia/core/lib/base/rand_util.h"

#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/stl_util.h"

namespace felicia {

std::string RandAlphaDigit(size_t length) {
  const char alphadigit[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  std::string text;
  text.resize(length);

  for (size_t i = 0; i < length; ++i) {
    text[i] = alphadigit[::base::RandGenerator(::base::size(alphadigit))];
  }

  return text;
}

}  // namespace felicia