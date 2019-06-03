#include <iostream>

#include "emscripten/bind.h"
#include "libyuv.h"

using namespace emscripten;

struct ColorIndex {
  int r_idx;
  int g_idx;
  int b_idx;
  int a_idx;
};

uintptr_t CreateBuffer(int size) {
  return reinterpret_cast<uintptr_t>(malloc(size));
}

void ReleaseBuffer(uintptr_t buffer) {
  free(reinterpret_cast<uint8_t*>(buffer));
}

void FillARGBColor(uint8_t* buffer, int width, int height,
                   const ColorIndex& color_index) {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      const int buffer_idx = 4 * (width * i + j);
      const uint8_t r = buffer[buffer_idx + color_index.r_idx];
      const uint8_t g = buffer[buffer_idx + color_index.g_idx];
      const uint8_t b = buffer[buffer_idx + color_index.b_idx];
      const uint8_t a = buffer[buffer_idx + color_index.a_idx];

      buffer[buffer_idx] = r;
      buffer[buffer_idx + 1] = g;
      buffer[buffer_idx + 2] = b;
      buffer[buffer_idx + 3] = a;
    }
  }
}

void FillARGBColor(uint8_t* buffer, int width, int height) {
  ColorIndex color_index;
  color_index.r_idx = 2;
  color_index.g_idx = 1;
  color_index.b_idx = 0;
  color_index.a_idx = 3;
  FillARGBColor(buffer, width, height, color_index);
}

void FillColor(uintptr_t start, size_t payload, uintptr_t argb_start, int width,
               int height, const std::string& pixel_format) {
  uint8_t* argb_buffer = reinterpret_cast<uint8_t*>(argb_start);
  if (pixel_format == "PIXEL_FORMAT_ARGB") {
    FillARGBColor(argb_buffer, width, height);
    return;
  }

  uint32_t src_format;
  if (pixel_format == "PIXEL_FORMAT_UYVY")
    src_format = libyuv::FOURCC_UYVY;
  else if (pixel_format == "PIXEL_FORMAT_YUY2")
    src_format = libyuv::FOURCC_YUY2;
  else if (pixel_format == "PIXEL_FORMAT_I420")
    src_format = libyuv::FOURCC_I420;
  else if (pixel_format == "PIXEL_FORMAT_RGB24")
    src_format = libyuv::FOURCC_24BG;
  else {
    std::cerr << "Unknown format: " << pixel_format << std::endl;
    return;
  }

  if (libyuv::ConvertToARGB(reinterpret_cast<uint8_t*>(start), payload,
                            argb_buffer, width * 4, 0 /* crop_x_pos */,
                            0 /* crop_y_pos */, width, height, width, height,
                            libyuv::RotationMode::kRotate0, src_format) != 0) {
    std::cerr << "Failed to convert to argb" << std::endl;
    return;
  }

  FillARGBColor(argb_buffer, width, height);
}

EMSCRIPTEN_BINDINGS(felicia_wasm) {
  function("createBuffer", &CreateBuffer);
  function("releaseBuffer", &ReleaseBuffer);
  function("fillColor", &FillColor);
}
