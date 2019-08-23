#include "felicia/slam/dataset/dataset_utils.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace slam {

StatusOr<double> TryConvertToDouble(const std::string& str,
                                    const base::FilePath& path, int line) {
  double n;
  if (!base::StringToDouble(str, &n)) {
    return errors::InvalidArgument(
        base::StringPrintf("Cann't convert to double %s at %" PRFilePath ":%d",
                           str.c_str(), path.value().c_str(), line));
  }
  return n;
}

}  // namespace slam
}  // namespace felicia
