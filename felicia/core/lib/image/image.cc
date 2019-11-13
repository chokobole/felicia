// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/image/image.h"

#include "third_party/chromium/base/files/file.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/core/lib/image/jpeg_codec.h"
#include "felicia/core/lib/image/png_codec.h"

namespace felicia {

Image::Image() = default;

Image::Image(Sizei size, PixelFormat pixel_format, const Data& data)
    : size_(size), pixel_format_(pixel_format), data_(data) {}

Image::Image(Sizei size, PixelFormat pixel_format, Data&& data)
    : size_(size), pixel_format_(pixel_format), data_(std::move(data)) {}

Image::Image(const Image& other) = default;

Image::Image(Image&& other) noexcept
    : size_(other.size_),
      pixel_format_(other.pixel_format_),
      data_(std::move(other.data_)) {}

Image& Image::operator=(const Image& other) = default;
Image& Image::operator=(Image&& other) = default;

Image::~Image() = default;

Sizei Image::size() const { return size_; }

int Image::width() const { return size_.width(); }

int Image::height() const { return size_.height(); }

void Image::set_size(Sizei size) { size_ = size; }

PixelFormat Image::pixel_format() const { return pixel_format_; }

void Image::set_pixel_format(PixelFormat pixel_format) {
  pixel_format_ = pixel_format;
}

Data& Image::data() { return data_; }

const Data& Image::data() const { return data_; }

ImageMessage Image::ToImageMessage(bool copy) {
  ImageMessage message;
  *message.mutable_size() = SizeiToSizeiMessage(size_);
  message.set_pixel_format(pixel_format_);
  if (copy) {
    message.set_data(data_.data());
  } else {
    message.set_data(std::move(data_).data());
  }
  return message;
}
Status Image::FromImageMessage(const ImageMessage& message) {
  *this = Image{SizeiMessageToSizei(message.size()), message.pixel_format(),
                Data{message.data()}};
  return Status::OK();
}

Status Image::FromImageMessage(ImageMessage&& message) {
  std::unique_ptr<std::string> data(message.release_data());
  *this = Image{SizeiMessageToSizei(message.size()), message.pixel_format(),
                Data{std::move(*data)}};
  return Status::OK();
}

Status Image::Save(const base::FilePath& path) const {
  std::vector<unsigned char> output;
  Status s;
  base::FilePath::StringType extension = path.Extension();
  if (extension == FILE_PATH_LITERAL(".jpg") ||
      extension == FILE_PATH_LITERAL(".jpeg")) {
    s = JpegCodec::Encode(*this, JpegCodec::Options(), &output);
  } else if (extension == FILE_PATH_LITERAL(".png")) {
    s = PngCodec::Encode(*this, PngCodec::Options(), &output);
  } else {
    return errors::InvalidArgument("Unsupported codec type.");
  }
  if (!s.ok()) return s;
  if (!WriteFile(path, reinterpret_cast<const char*>(output.data()),
                 output.size()))
    return errors::InvalidArgument("Failed to write file.");
  return Status::OK();
}

Status Image::Load(const base::FilePath& path, PixelFormat pixel_format) {
  std::unique_ptr<char[]> input;
  size_t input_len;
  if (!ReadFile(path, &input, &input_len))
    return errors::InvalidArgument("Failed to read file.");

  pixel_format_ = pixel_format;
  base::FilePath::StringType extension = path.Extension();
  if (extension == FILE_PATH_LITERAL(".jpg") ||
      extension == FILE_PATH_LITERAL(".jpeg")) {
    return JpegCodec::Decode(reinterpret_cast<uint8_t*>(input.get()), input_len,
                             this);
  } else if (extension == FILE_PATH_LITERAL(".png")) {
    return PngCodec::Decode(reinterpret_cast<uint8_t*>(input.get()), input_len,
                            this);
  } else {
    return errors::InvalidArgument("Unsupported codec type.");
  }
}

}  // namespace felicia