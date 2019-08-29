// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/ui/gfx/codec/png_codec.cc

#include "felicia/core/lib/image/png_codec.h"

#include "png.h"
#include "zlib.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

// Encoder --------------------------------------------------------------------
//
// This section of the code is based on nsPNGEncoder.cpp in Mozilla
// (Copyright 2005 Google Inc.)

namespace {

// Passed around as the io_ptr in the png structs so our callbacks know where
// to write data.
class PngEncoderState {
 public:
  PngEncoderState(std::vector<unsigned char>* o) : output(o) {}
  std::vector<unsigned char>* output;

 private:
  DISALLOW_COPY_AND_ASSIGN(PngEncoderState);
};

// Called by libpng to flush its internal buffer to ours.
void EncoderWriteCallback(png_structp png, png_bytep data, png_size_t size) {
  PngEncoderState* state = static_cast<PngEncoderState*>(png_get_io_ptr(png));
  size_t cur_size = state->output->size();
  state->output->resize(cur_size + size);
  memcpy(state->output->data() + cur_size, data, size);
}

// Holds png struct and info ensuring the proper destruction.
class PngWriteStructInfo {
 public:
  PngWriteStructInfo() : png_ptr_(nullptr), info_ptr_(nullptr) {}
  ~PngWriteStructInfo() { png_destroy_write_struct(&png_ptr_, &info_ptr_); }

  Status Build() {
    png_ptr_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_) return errors::Unavailable("Failed to create write struct.");

    info_ptr_ = png_create_info_struct(png_ptr_);
    if (!info_ptr_) return errors::Unavailable("Failed to create info struct.");

    return Status::OK();
  }

  png_struct* png_ptr_;
  png_info* info_ptr_;

 private:
  DISALLOW_COPY_AND_ASSIGN(PngWriteStructInfo);
};

}  // namespace

// static
Status PngCodec::Encode(const Image& image, const Options& options,
                        std::vector<unsigned char>* output) {
  output->clear();
  PngWriteStructInfo si;
  Status s = si.Build();
  if (!s.ok()) return s;

  if (setjmp(png_jmpbuf(si.png_ptr_))) {
    // The destroyer will ensure that the structures are cleaned up in this
    // case, even though we may get here as a jump from random parts of the
    // PNG library called below.
    return errors::Unknown("Failed to decode.");
  }

  PngEncoderState state(output);
  png_set_write_fn(si.png_ptr_, &state, EncoderWriteCallback, 0);

  int output_channels;
  int output_color_type;
  switch (image.pixel_format()) {
    case PIXEL_FORMAT_BGRA:
      output_channels = 4;
      output_color_type = PNG_COLOR_TYPE_RGBA;
      png_set_bgr(si.png_ptr_);
      break;
    case PIXEL_FORMAT_BGR:
      output_channels = 3;
      output_color_type = PNG_COLOR_TYPE_RGB;
      png_set_bgr(si.png_ptr_);
      break;
    case PIXEL_FORMAT_Y8:
      output_channels = 3;
      output_color_type = PNG_COLOR_TYPE_RGB;
      png_set_gray_to_rgb(si.png_ptr_);
      break;
    case PIXEL_FORMAT_Y16:
      output_channels = 3;
      output_color_type = PNG_COLOR_TYPE_RGB;
      png_set_gray_to_rgb(si.png_ptr_);
      png_set_strip_16(si.png_ptr_);
      break;
    case PIXEL_FORMAT_RGBA:
      output_color_type = PNG_COLOR_TYPE_RGBA;
      output_channels = 4;
      break;
    case PIXEL_FORMAT_RGB:
      output_color_type = PNG_COLOR_TYPE_RGB;
      output_channels = 3;
      break;
    default:
      NOTREACHED();
      break;
  }

  png_set_IHDR(si.png_ptr_, si.info_ptr_, image.width(), image.height(), 8,
               output_color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  int compression_level =
      std::max(std::min(options.compression_level, Z_BEST_COMPRESSION), 0);
  png_set_compression_level(si.png_ptr_, compression_level);
  png_write_info(si.png_ptr_, si.info_ptr_);

  const unsigned char* rowptr = image.data().cast<const unsigned char*>();
  int row_read_stride = image.width() * output_channels;
  for (int row = 0; row < image.height(); row++, rowptr += row_read_stride) {
    png_write_row(si.png_ptr_, rowptr);
  }

  png_write_end(si.png_ptr_, si.info_ptr_);
  return Status::OK();
}

// Decoder --------------------------------------------------------------------
//
// This code is based on WebKit libpng interface (PNGImageDecoder), which is
// in turn based on the Mozilla png decoder.

namespace {

// Gamma constants: We assume we're on Windows which uses a gamma of 2.2.
const double kMaxGamma = 21474.83;  // Maximum gamma accepted by png library.
const double kDefaultGamma = 2.2;
const double kInverseGamma = 1.0 / kDefaultGamma;

class PngDecoderState {
 public:
  explicit PngDecoderState(Image* image)
      : output_format(image->pixel_format()),
        output_channels(0),
        image(image),
        is_opaque(true),
        done(false) {}

  PixelFormat output_format;
  int output_channels;

  // An incoming Image to write to.
  Image* image;

  // Used during the reading of an SkBitmap. Defaults to true until we see a
  // pixel with anything other than an alpha of 255.
  bool is_opaque;

  // Set to true when we've found the end of the data.
  bool done;

 private:
  DISALLOW_COPY_AND_ASSIGN(PngDecoderState);
};

// Called when the png header has been read. This code is based on the WebKit
// PNGImageDecoder
void DecodeInfoCallback(png_struct* png_ptr, png_info* info_ptr) {
  PngDecoderState* state =
      static_cast<PngDecoderState*>(png_get_progressive_ptr(png_ptr));

  int bit_depth, color_type, interlace_type, compression_type;
  int filter_type;
  png_uint_32 w, h;
  png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_type);

  // Bounds check. When the image is unreasonably big, we'll error out and
  // end up back at the setjmp call when we set up decoding.  "Unreasonably big"
  // means "big enough that w * h * 32bpp might overflow an int"; we choose this
  // threshold to match WebKit and because a number of places in code assume
  // that an image's size (in bytes) fits in a (signed) int.
  unsigned long long total_size =
      static_cast<unsigned long long>(w) * static_cast<unsigned long long>(h);
  if (total_size > ((1 << 29) - 1)) longjmp(png_jmpbuf(png_ptr), 1);
  state->image->set_size(Sizei{static_cast<int>(w), static_cast<int>(h)});

  // The following png_set_* calls have to be done in the order dictated by
  // the libpng docs. Please take care if you have to move any of them. This
  // is also why certain things are done outside of the switch, even though
  // they look like they belong there.

  // Expand to ensure we use 24-bit for RGB and 32-bit for RGBA.
  if (color_type == PNG_COLOR_TYPE_PALETTE ||
      (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8))
    png_set_expand(png_ptr);

  // The '!= 0' is for silencing a Windows compiler warning.
  bool input_has_alpha = ((color_type & PNG_COLOR_MASK_ALPHA) != 0);

  // Transparency for paletted images.
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_expand(png_ptr);
    input_has_alpha = true;
  }

  // Convert 16-bit to 8-bit.
  if (bit_depth == 16) png_set_strip_16(png_ptr);

  // Pick our row format converter necessary for this data.
  if (!input_has_alpha) {
    switch (state->output_format) {
      case PIXEL_FORMAT_BGRA:
        state->output_channels = 4;
        png_set_bgr(png_ptr);
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
        break;
      case PIXEL_FORMAT_BGR:
        state->output_channels = 3;
        png_set_bgr(png_ptr);
        break;
      case PIXEL_FORMAT_RGBA:
        state->output_channels = 4;
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
        break;
      case PIXEL_FORMAT_RGB:
        state->output_channels = 3;
        break;
      default:
        NOTREACHED();
        break;
    }
  } else {
    state->output_channels = 4;
    switch (state->output_format) {
      case PIXEL_FORMAT_BGR:
      case PIXEL_FORMAT_BGRA:
        png_set_bgr(png_ptr);
        break;
      case PIXEL_FORMAT_RGB:
      case PIXEL_FORMAT_RGBA:
        break;
      default:
        NOTREACHED();
        break;
    }
  }

  // Expand grayscale to RGB.
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  // Deal with gamma and keep it under our control.
  double gamma;
  if (png_get_gAMA(png_ptr, info_ptr, &gamma)) {
    if (gamma <= 0.0 || gamma > kMaxGamma) {
      gamma = kInverseGamma;
      png_set_gAMA(png_ptr, info_ptr, gamma);
    }
    png_set_gamma(png_ptr, kDefaultGamma, gamma);
  } else {
    png_set_gamma(png_ptr, kDefaultGamma, kInverseGamma);
  }

  // Tell libpng to send us rows for interlaced pngs.
  if (interlace_type == PNG_INTERLACE_ADAM7)
    png_set_interlace_handling(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  state->image->data().resize(state->image->size().area() *
                              state->output_channels);
}

void DecodeRowCallback(png_struct* png_ptr, png_byte* new_row,
                       png_uint_32 row_num, int pass) {
  if (!new_row) return;  // Interlaced image; row didn't change this pass.

  PngDecoderState* state =
      static_cast<PngDecoderState*>(png_get_progressive_ptr(png_ptr));

  if (static_cast<int>(row_num) > state->image->size().height()) {
    NOTREACHED() << "Invalid row";
    return;
  }

  unsigned char* base = state->image->data().cast<unsigned char*>();
  unsigned char* dest =
      &base[state->image->width() * state->output_channels * row_num];
  png_progressive_combine_row(png_ptr, dest, new_row);
}

void DecodeEndCallback(png_struct* png_ptr, png_info* info) {
  PngDecoderState* state =
      static_cast<PngDecoderState*>(png_get_progressive_ptr(png_ptr));

  // Mark the image as complete, this will tell the Decode function that we
  // have successfully found the end of the data.
  state->done = true;
}

// Holds png struct and info ensuring the proper destruction.
class PngReadStructInfo {
 public:
  PngReadStructInfo() : png_ptr_(nullptr), info_ptr_(nullptr) {}
  ~PngReadStructInfo() { png_destroy_read_struct(&png_ptr_, &info_ptr_, NULL); }

  Status Build(const unsigned char* input, size_t input_size) {
    if (input_size < 8)
      return errors::InvalidArgument("Input data is too small to be a png.");

    // Have libpng check the signature, it likes the first 8 bytes.
    if (png_sig_cmp(const_cast<unsigned char*>(input), 0, 8) != 0)
      return errors::InvalidArgument("Failed to check the signature.");

    png_ptr_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_) return errors::Unavailable("Failed to create read struct.");

    info_ptr_ = png_create_info_struct(png_ptr_);
    if (!info_ptr_) return errors::Unavailable("Failed to create info struct.");

    return Status::OK();
  }

  png_struct* png_ptr_;
  png_info* info_ptr_;

 private:
  DISALLOW_COPY_AND_ASSIGN(PngReadStructInfo);
};

}  // namespace

// static
Status PngCodec::Decode(const unsigned char* input, size_t input_size,
                        Image* image) {
  DCHECK(image);
  PngReadStructInfo si;
  Status s = si.Build(input, input_size);
  if (!s.ok()) return s;

  if (setjmp(png_jmpbuf(si.png_ptr_))) {
    // The destroyer will ensure that the structures are cleaned up in this
    // case, even though we may get here as a jump from random parts of the
    // PNG library called below.
    return errors::Unknown("Failed to decode.");
  }

  PngDecoderState state(image);

  png_set_progressive_read_fn(si.png_ptr_, &state, &DecodeInfoCallback,
                              &DecodeRowCallback, &DecodeEndCallback);
  png_process_data(si.png_ptr_, si.info_ptr_, const_cast<unsigned char*>(input),
                   input_size);

  if (!state.done) {
    return errors::InvalidArgument("Libpng needs more data.");
  }

  return Status::OK();
}

}  // namespace felicia