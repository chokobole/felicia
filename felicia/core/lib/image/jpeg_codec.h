// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/ui/gfx/codec/jpeg_codec.h

#ifndef FELICIA_CORE_LIB_IMAGE_JPEG_CODEC_H_
#define FELICIA_CORE_LIB_IMAGE_JPEG_CODEC_H_

#include <stdint.h>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/image/image.h"

namespace felicia {

class EXPORT JpegCodec {
 public:
  static Status Decode(const uint8_t* input, size_t input_size, Image* image);
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_IMAGE_JPEG_CODEC_H_