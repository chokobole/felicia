// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/ui/gfx/codec/jpeg_codec.cc

#include "felicia/core/lib/image/jpeg_codec.h"

#include <setjmp.h>

#include "jpeglib.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

// Encoder/decoder shared stuff ------------------------------------------------

namespace {

// used to pass error info through the JPEG library
struct CoderErrorMgr {
  jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

void ErrorExit(jpeg_common_struct* cinfo) {
  CoderErrorMgr* err = reinterpret_cast<CoderErrorMgr*>(cinfo->err);

  // Return control to the setjmp point.
  longjmp(err->setjmp_buffer, false);
}

}  // namespace

// Encoder --------------------------------------------------------------------

namespace {

struct JpegDestinationMgr : jpeg_destination_mgr {
  static constexpr size_t kBufferSize = 1024;
  std::vector<unsigned char>* destination;
  unsigned char buffer[kBufferSize];
};

// Callback to initialize to the destination.
//
// From the JPEG library:
// "Initialize destination. This is called by jpeg_start_compress() before any
//  data is actually written. It must initialize next_output_byte and
//  free_in_buffer. free_in_buffer must be initialized to a positive value."
void InitDestination(j_compress_ptr cinfo) {
  JpegDestinationMgr* dest = static_cast<JpegDestinationMgr*>(cinfo->dest);
  dest->next_output_byte = dest->buffer;
  dest->free_in_buffer = JpegDestinationMgr::kBufferSize;
}

// Callback to empty the buffer.
//
// From the JPEG library:
// "This is called whenever the buffer has filled (free_in_buffer reaches zero).
//  In typical applications, it should write out the entire buffer
//  (use the saved start address and buffer length; ignore the current state of
//  next_output_byte and free_in_buffer). Then reset the pointer & count to the
//  start of the buffer, and return TRUE indicating that the buffer has been
//  dumped. free_in_buffer must be set to a positive value when TRUE is
//  returned. A FALSE return should only be used when I/O suspension is desired
//  (this operating mode is discussed in the next section)."
boolean EmptyOutputBuffer(j_compress_ptr cinfo) {
  JpegDestinationMgr* dest = static_cast<JpegDestinationMgr*>(cinfo->dest);
  size_t cur_size = dest->destination->size();
  dest->destination->resize(cur_size + JpegDestinationMgr::kBufferSize);
  memcpy(dest->destination->data() + cur_size, dest->buffer,
         JpegDestinationMgr::kBufferSize);

  dest->next_output_byte = dest->buffer;
  dest->free_in_buffer = JpegDestinationMgr::kBufferSize;
  return TRUE;
}

// Compression is finished, so we finally move the output buffer to the
// destinatino buffer.
//
// From the JPEG library:
// "Terminate destination --- called by jpeg_finish_compress() after all data
//  has been written. In most applications, this must flush any data remaining
//  in the buffer. Use either next_output_byte or free_in_buffer to determine
//  how much data is in the buffer."
void TermDestination(j_compress_ptr cinfo) {
  JpegDestinationMgr* dest = static_cast<JpegDestinationMgr*>(cinfo->dest);
  size_t size = JpegDestinationMgr::kBufferSize - dest->free_in_buffer;
  if (size > 0) {
    size_t cur_size = dest->destination->size();
    dest->destination->resize(cur_size + size);
    memcpy(dest->destination->data() + cur_size, dest->buffer, size);
  }
}

// jpeg_compress_struct Deleter.
struct JpegCompressStructDeleter {
  void operator()(jpeg_compress_struct* ptr) {
    jpeg_destroy_compress(ptr);
    delete ptr;
  }
};

}  // namespace

// static
Status JpegCodec::Encode(const Image& image, const Options& options,
                         std::vector<unsigned char>* output) {
  std::unique_ptr<jpeg_compress_struct, JpegCompressStructDeleter> cinfo(
      new jpeg_compress_struct);
  output->clear();

  // We set up the normal JPEG error routines, then override error_exit.
  // This must be done before the call to jpeg_create_compress.
  CoderErrorMgr errmgr;
  cinfo->err = jpeg_std_error(&errmgr.pub);
  errmgr.pub.error_exit = ErrorExit;
  // Establish the setjmp return context for ErrorExit to use.
  if (setjmp(errmgr.setjmp_buffer)) {
    // If we get here, the JPEG code has signaled an error.
    // Release |cinfo| by hand to avoid use-after-free of |errmgr|.
    cinfo.reset();
    return errors::Unknown("Failed to encode.");
  }

  // The destroyer will destroy() cinfo on exit.  We don't want to set the
  // destroyer's object until cinfo is initialized.
  jpeg_create_compress(cinfo.get());

  // set up the destination manager
  JpegDestinationMgr dstmgr;
  dstmgr.destination = output;
  dstmgr.init_destination = InitDestination;
  dstmgr.empty_output_buffer = EmptyOutputBuffer;
  dstmgr.term_destination = TermDestination;
  cinfo->dest = &dstmgr;

  cinfo->image_width = static_cast<JDIMENSION>(image.width());
  cinfo->image_height = static_cast<JDIMENSION>(image.height());

  switch (image.pixel_format()) {
    case PIXEL_FORMAT_BGRA:
      cinfo->in_color_space = JCS_EXT_BGRA;
      cinfo->input_components = 4;
      break;
    case PIXEL_FORMAT_BGR:
      cinfo->in_color_space = JCS_EXT_BGR;
      cinfo->input_components = 3;
      break;
    case PIXEL_FORMAT_BGRX:
      cinfo->in_color_space = JCS_EXT_BGRX;
      cinfo->input_components = 4;
      break;
    case PIXEL_FORMAT_Y8:
      cinfo->in_color_space = JCS_GRAYSCALE;
      cinfo->input_components = 1;
      break;
    case PIXEL_FORMAT_RGBA:
      cinfo->in_color_space = JCS_EXT_RGBA;
      cinfo->input_components = 4;
      break;
    case PIXEL_FORMAT_RGBX:
      cinfo->in_color_space = JCS_EXT_RGBX;
      cinfo->input_components = 4;
      break;
    case PIXEL_FORMAT_RGB:
      cinfo->in_color_space = JCS_RGB;
      cinfo->input_components = 3;
      break;
    case PIXEL_FORMAT_ARGB:
      cinfo->in_color_space = JCS_EXT_ARGB;
      cinfo->input_components = 4;
      break;
    default:
      return errors::InvalidArgument("Invalid pixel format.");
  }

  jpeg_set_defaults(cinfo.get());
  cinfo->optimize_coding = TRUE;

  jpeg_set_quality(cinfo.get(), options.quality, TRUE);
  jpeg_start_compress(cinfo.get(), TRUE);

  int row_read_stride = cinfo->image_width * cinfo->input_components;

  const unsigned char* rowptr = image.data().cast<const unsigned char*>();
  for (int row = 0; row < static_cast<int>(cinfo->image_height);
       row++, rowptr += row_read_stride) {
    if (!jpeg_write_scanlines(cinfo.get(), const_cast<unsigned char**>(&rowptr),
                              1))
      return errors::InvalidArgument("Failed read scanlines.");
  }

  jpeg_finish_compress(cinfo.get());
  return Status::OK();
}

// Decoder --------------------------------------------------------------------

namespace {

struct JpegDecoderState {
  JpegDecoderState(const unsigned char* in, size_t len)
      : input_buffer(in), input_buffer_length(len) {}

  const unsigned char* input_buffer;
  size_t input_buffer_length;
};

// Callback to initialize the source.
//
// From the JPEG library:
//  "Initialize source. This is called by jpeg_read_header() before any data is
//   actually read. May leave bytes_in_buffer set to 0 (in which case a
//   fill_input_buffer() call will occur immediately)."
void InitSource(j_decompress_ptr cinfo) {
  JpegDecoderState* state = static_cast<JpegDecoderState*>(cinfo->client_data);
  cinfo->src->next_input_byte = state->input_buffer;
  cinfo->src->bytes_in_buffer = state->input_buffer_length;
}

// Callback to fill the buffer. Since our buffer already contains all the data,
// we should never need to provide more data. If libjpeg thinks it needs more
// data, our input is probably corrupt.
//
// From the JPEG library:
//  "This is called whenever bytes_in_buffer has reached zero and more data is
//   wanted. In typical applications, it should read fresh data into the buffer
//   (ignoring the current state of next_input_byte and bytes_in_buffer), reset
//   the pointer & count to the start of the buffer, and return TRUE indicating
//   that the buffer has been reloaded. It is not necessary to fill the buffer
//   entirely, only to obtain at least one more byte. bytes_in_buffer MUST be
//   set to a positive value if TRUE is returned. A FALSE return should only
//   be used when I/O suspension is desired."
boolean FillInputBuffer(j_decompress_ptr cinfo) { return false; }

// Skip data in the buffer. Since we have all the data at once, this operation
// is easy. It is not clear if this ever gets called because the JPEG library
// should be able to do the skip itself (it has all the data).
//
// From the JPEG library:
//  "Skip num_bytes worth of data. The buffer pointer and count should be
//   advanced over num_bytes input bytes, refilling the buffer as needed. This
//   is used to skip over a potentially large amount of uninteresting data
//   (such as an APPn marker). In some applications it may be possible to
//   optimize away the reading of the skipped data, but it's not clear that
//   being smart is worth much trouble; large skips are uncommon.
//   bytes_in_buffer may be zero on return. A zero or negative skip count
//   should be treated as a no-op."
void SkipInputData(j_decompress_ptr cinfo, long num_bytes) {
  if (num_bytes > static_cast<long>(cinfo->src->bytes_in_buffer)) {
    // Since all our data should be in the buffer, trying to skip beyond it
    // means that there is some kind of error or corrupt input data. A 0 for
    // bytes left means it will call FillInputBuffer which will then fail.
    cinfo->src->next_input_byte += cinfo->src->bytes_in_buffer;
    cinfo->src->bytes_in_buffer = 0;
  } else if (num_bytes > 0) {
    cinfo->src->bytes_in_buffer -= static_cast<size_t>(num_bytes);
    cinfo->src->next_input_byte += num_bytes;
  }
}

// Our source doesn't need any cleanup, so this is a NOP.
//
// From the JPEG library:
//  "Terminate source --- called by jpeg_finish_decompress() after all data has
//   been read to clean up JPEG source manager. NOT called by jpeg_abort() or
//   jpeg_destroy()."
void TermSource(j_decompress_ptr cinfo) {}

// jpeg_decompress_struct Deleter.
struct JpegDecompressStructDeleter {
  void operator()(jpeg_decompress_struct* ptr) {
    jpeg_destroy_decompress(ptr);
    delete ptr;
  }
};

}  // namespace

// static
Status JpegCodec::Decode(const unsigned char* input, size_t input_size,
                         Image* image) {
  std::unique_ptr<jpeg_decompress_struct, JpegDecompressStructDeleter> cinfo(
      new jpeg_decompress_struct);
  image->data().clear();

  // We set up the normal JPEG error routines, then override error_exit.
  // This must be done before the call to jpeg_create_decompress.
  CoderErrorMgr errmgr;
  cinfo->err = jpeg_std_error(&errmgr.pub);
  errmgr.pub.error_exit = ErrorExit;
  // Establish the setjmp return context for ErrorExit to use.
  if (setjmp(errmgr.setjmp_buffer)) {
    // If we get here, the JPEG code has signaled an error.
    // Release |cinfo| by hand to avoid use-after-free of |errmgr|.
    cinfo.reset();
    return errors::Unknown("Failed to decode.");
  }

  // The destroyer will destroy() cinfo on exit.  We don't want to set the
  // destroyer's object until cinfo is initialized.
  jpeg_create_decompress(cinfo.get());

  // set up the source manager
  jpeg_source_mgr srcmgr;
  srcmgr.init_source = InitSource;
  srcmgr.fill_input_buffer = FillInputBuffer;
  srcmgr.skip_input_data = SkipInputData;
  srcmgr.resync_to_restart = jpeg_resync_to_restart;  // use default routine
  srcmgr.term_source = TermSource;
  cinfo->src = &srcmgr;

  JpegDecoderState state(input, input_size);
  cinfo->client_data = &state;

  // fill the file metadata into our buffer
  if (jpeg_read_header(cinfo.get(), true) != JPEG_HEADER_OK)
    return errors::InvalidArgument("Failed to read header.");

  // we want to always get RGB data out
  switch (cinfo->jpeg_color_space) {
    case JCS_GRAYSCALE:
    case JCS_RGB:
    case JCS_YCbCr:
      // Choose an output colorspace and return if it is an unsupported one.
      // Same as JPEGCodec::Encode(), libjpeg-turbo supports all input formats
      // used by Chromium (i.e. RGBA and BGRA) and we just map the input
      // parameters to a colorspace.
      switch (image->pixel_format()) {
        case PIXEL_FORMAT_BGRA:
          cinfo->out_color_space = JCS_EXT_BGRA;
          cinfo->output_components = 4;
          break;
        case PIXEL_FORMAT_BGR:
          cinfo->out_color_space = JCS_EXT_BGR;
          cinfo->output_components = 3;
          break;
        case PIXEL_FORMAT_BGRX:
          cinfo->out_color_space = JCS_EXT_BGRX;
          cinfo->output_components = 4;
          break;
        case PIXEL_FORMAT_Y8:
          cinfo->out_color_space = JCS_GRAYSCALE;
          cinfo->output_components = 1;
          break;
        case PIXEL_FORMAT_RGBA:
          cinfo->out_color_space = JCS_EXT_RGBA;
          cinfo->output_components = 4;
          break;
        case PIXEL_FORMAT_RGBX:
          cinfo->out_color_space = JCS_EXT_RGBX;
          cinfo->output_components = 4;
          break;
        case PIXEL_FORMAT_RGB:
          cinfo->out_color_space = JCS_RGB;
          cinfo->output_components = 3;
          break;
        case PIXEL_FORMAT_ARGB:
          cinfo->out_color_space = JCS_EXT_ARGB;
          cinfo->output_components = 4;
          break;
        default:
          return errors::InvalidArgument("Invalid pixel format.");
      }
      break;
    case JCS_CMYK:
    case JCS_YCCK:
    default:
      // Mozilla errors out on these color spaces, so I presume that the jpeg
      // library can't do automatic color space conversion for them. We don't
      // care about these anyway.
      return errors::InvalidArgument("Invalid jpeg color space.");
  }

  jpeg_calc_output_dimensions(cinfo.get());
  image->set_size(Sizei{static_cast<int>(cinfo->output_width),
                        static_cast<int>(cinfo->output_height)});

  jpeg_start_decompress(cinfo.get());

  // FIXME(brettw) we may want to allow the capability for callers to request
  // how to align row lengths as we do for the compressor.
  int row_read_stride = cinfo->output_width * cinfo->output_components;

  // Create memory for a decoded image and write decoded lines to the memory
  // without conversions same as JPEGCodec::Encode().
  int row_write_stride = row_read_stride;
  image->data().resize(row_write_stride * cinfo->output_height);

  unsigned char* rowptr = image->data().cast<unsigned char*>();
  for (int row = 0; row < static_cast<int>(cinfo->output_height);
       row++, rowptr += row_write_stride) {
    if (!jpeg_read_scanlines(cinfo.get(), &rowptr, 1))
      return errors::InvalidArgument("Failed read scanlines.");
  }

  jpeg_finish_decompress(cinfo.get());
  return Status::OK();
}

}  // namespace felicia