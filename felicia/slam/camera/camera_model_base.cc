#include "felicia/slam/camera/camera_model_base.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace slam {
namespace internal {

Status InvalidRowsAndCols(int rows, int cols) {
  return errors::InvalidArgument(
      base::StringPrintf("rows and cols are not invalid. (%dx%d)", rows, cols));
}

Status InvalidImageSize(int width, int height) {
  return errors::InvalidArgument(
      base::StringPrintf("image size is not valid. (%dx%d)", width, height));
}

bool IsValidImageSize(int width, int height) {
  return width >= 0 && height >= 0;
}

Status MaybeLoad(const YAML::Node& node, const std::string& name,
                 const base::FilePath& path,
                 std::function<Status(const YAML::Node&)> callback) {
  YAML::Node n = node[name];
  if (n) {
    return callback(n);
  } else {
    LOG(WARNING) << "Missing \"" << name << "\" field in \"" << path << "\"";
  }
  return Status::OK();
}

}  // namespace internal
}  // namespace slam
}  // namespace felicia