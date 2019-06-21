#include <iostream>

#include "emscripten/bind.h"
#include "libyuv.h"

using namespace emscripten;

uintptr_t CreateBuffer(int size) {
  return reinterpret_cast<uintptr_t>(malloc(size));
}

void ReleaseBuffer(uintptr_t buffer) {
  free(reinterpret_cast<uint8_t*>(buffer));
}

bool ConvertToARGB(uintptr_t start, size_t payload, uintptr_t argb_start,
                   int width, int height, const std::string& pixel_format) {
  if (pixel_format == "PIXEL_FORMAT_ARGB") {
    std::cerr << "Its format is already PIXEL_FORMAT_ARGB." << std::endl;
  }

  libyuv::FourCC src_format;
  if (pixel_format == "PIXEL_FORMAT_I420")
    src_format = libyuv::FOURCC_I420;
  else if (pixel_format == "PIXEL_FORMAT_YV12")
    src_format = libyuv::FOURCC_YV12;
  else if (pixel_format == "PIXEL_FORMAT_NV12")
    src_format = libyuv::FOURCC_NV12;
  if (pixel_format == "PIXEL_FORMAT_UYVY")
    src_format = libyuv::FOURCC_UYVY;
  else if (pixel_format == "PIXEL_FORMAT_YUY2")
    src_format = libyuv::FOURCC_YUY2;
  else if (pixel_format == "PIXEL_FORMAT_ARGB")
    src_format = libyuv::FOURCC_ARGB;
  else if (pixel_format == "PIXEL_FORMAT_RGB24")
    src_format = libyuv::FOURCC_24BG;
  else if (pixel_format == "PIXEL_FORMAT_RGB32")
    src_format = libyuv::FOURCC_BGRA;
  else if (pixel_format == "PIXEL_FORMAT_MJPEG")
    src_format = libyuv::FOURCC_MJPG;
  else if (pixel_format == "PIXEL_FORMAT_ABGR")
    src_format = libyuv::FOURCC_ABGR;
  else if (pixel_format == "PIXEL_FORMAT_XBGR")
    src_format = libyuv::FOURCC_RAW;
  else {
    std::cerr << "Unknown format: " << pixel_format << std::endl;
    return false;
  }

  if (libyuv::ConvertToARGB(reinterpret_cast<uint8_t*>(start), payload,
                            reinterpret_cast<uint8_t*>(argb_start), width * 4,
                            0 /* crop_x_pos */, 0 /* crop_y_pos */, width,
                            height, width, height,
                            libyuv::RotationMode::kRotate0, src_format) != 0) {
    std::cerr << "Failed to convert to argb" << std::endl;
    return false;
  }

  return true;
}

EMSCRIPTEN_BINDINGS(felicia_wasm) {
  function("createBuffer", &CreateBuffer);
  function("releaseBuffer", &ReleaseBuffer);
  function("convertToARGB", &ConvertToARGB);
}
