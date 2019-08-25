#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

#include <iostream>

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

CameraFlag::CameraFlag(int default_width, int default_height, float default_fps,
                       int default_pixel_format) {
  {
    BoolFlag::Builder builder(MakeValueStore(&device_list_));

    auto flag = builder.SetShortName("-l")
                    .SetLongName("--device_list")
                    .SetHelp(
                        "whether it shows device list, If you don't know which"
                        " device index to pass, then try this option!")
                    .Build();
    device_list_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    Flag<size_t>::Builder builder(MakeValueStore(&device_index_));

    auto flag = builder.SetShortName("-i")
                    .SetLongName("--device_index")
                    .SetHelp("device index for camera")
                    .Build();
    device_index_flag_ = std::make_unique<Flag<size_t>>(flag);
  }
  {
    IntDefaultFlag::Builder builder(MakeValueStore(&width_, default_width));
    auto flag = builder.SetShortName("-w")
                    .SetLongName("--width")
                    .SetHelp(base::StringPrintf(
                        "camera frame width (default: %d)", default_width))
                    .Build();
    width_flag_ = std::make_unique<IntDefaultFlag>(flag);
  }
  {
    IntDefaultFlag::Builder builder(MakeValueStore(&height_, default_height));
    auto flag = builder.SetShortName("-h")
                    .SetLongName("--height")
                    .SetHelp(base::StringPrintf(
                        "camera frame height (default: %d)", default_height))
                    .Build();
    height_flag_ = std::make_unique<IntDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&fps_, default_fps));
    auto flag = builder.SetShortName("-f")
                    .SetLongName("--fps")
                    .SetHelp(base::StringPrintf(
                        "frame per second (default: %f)", default_fps))
                    .Build();
    fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    PixelFormat kDefaultPixelFormats[] = {
        PIXEL_FORMAT_BGR,
        PIXEL_FORMAT_BGRA,
        PIXEL_FORMAT_RGB,
        PIXEL_FORMAT_RGBA,
    };

    PixelFormat final_default_pixel_format = PIXEL_FORMAT_BGR;
    for (auto& pixel_format : kDefaultPixelFormats) {
      if (pixel_format == default_pixel_format) {
        final_default_pixel_format = pixel_format;
        break;
      }
    }

    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &channel_type_, PixelFormat_Name(final_default_pixel_format),
        Choices<std::string>{PixelFormat_Name(PIXEL_FORMAT_BGR),
                             PixelFormat_Name(PIXEL_FORMAT_BGRA),
                             PixelFormat_Name(PIXEL_FORMAT_RGB),
                             PixelFormat_Name(PIXEL_FORMAT_RGBA)}));
    auto flag = builder.SetLongName("--pixel_format")
                    .SetHelp(base::StringPrintf(
                        "pixel_format (default: %s)",
                        PixelFormat_Name(final_default_pixel_format).c_str()))
                    .Build();
    pixel_format_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
}

CameraFlag::~CameraFlag() = default;

bool CameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, is_publishing_node_flag_, name_flag_,
                             topic_flag_, device_list_flag_, device_index_flag_,
                             width_flag_, height_flag_, fps_flag_,
                             pixel_format_flag_);
}

bool CameraFlag::Validate() const {
  if (CheckIfDeviceListFlagWasSet()) return true;

  if (!NodeCreateFlag::Validate()) return false;

  return CheckIfCameraFlagsValid(is_publishing_node_);
}

bool CameraFlag::CheckIfDeviceListFlagWasSet() const {
  if (device_list_flag_->is_set()) {
    std::cout << "device_list is on, it just shows a list of camera devices. "
                 "If you pass -i(--device_index) with the -l then you can "
                 "iterate the camera formats the device supports."
              << std::endl;
    return true;
  }
  return false;
}

bool CameraFlag::CheckIfCameraFlagsValid(bool is_publishing_node) const {
  if (is_publishing_node) {
    if (!(CheckIfFlagPositive(width_flag_) &&
          CheckIfFlagPositive(height_flag_) && CheckIfFlagPositive(fps_flag_)))
      return false;

    return CheckIfFlagWasSet(device_index_flag_);
  } else {
    return CheckIfFlagPositive(fps_flag_);
  }
}

}  // namespace felicia