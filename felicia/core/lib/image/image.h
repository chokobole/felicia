#ifndef FELICIA_CORE_LIB_IMAGE_IMAGE_H_
#define FELICIA_CORE_LIB_IMAGE_IMAGE_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/string_vector.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/core/protobuf/ui.pb.h"

namespace felicia {

class EXPORT Image {
 public:
  Image();
  Image(Sizei size, PixelFormat pixel_format, const std::string& data);
  Image(Sizei size, PixelFormat pixel_format, std::string&& data);
  Image(const Image& other);
  Image(Image&& other) noexcept;
  Image& operator=(const Image& other);
  Image& operator=(Image&& other);
  ~Image();

  Sizei size() const;
  int width() const;
  int height() const;
  void set_size(Sizei size);
  PixelFormat pixel_format() const;
  void set_pixel_format(PixelFormat pixel_format);

  StringVector& data();
  const StringVector& data() const;

  ImageMessage ToImageMessage(bool copy = true);
  Status FromImageMessage(const ImageMessage& message);
  Status FromImageMessage(ImageMessage&& message);

  Status Load(const base::FilePath& path,
              PixelFormat pixel_format = PIXEL_FORMAT_RGB);

 private:
  Sizei size_;
  PixelFormat pixel_format_;
  StringVector data_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_IMAGE_IMAGE_H_