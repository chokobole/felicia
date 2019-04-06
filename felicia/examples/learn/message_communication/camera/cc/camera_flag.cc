#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"

#include <iostream>

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

CameraFlag::CameraFlag() {
  {
#if defined(OS_LINUX)
    std::string device_id = "/dev/video0";
#else
    std::string device_id = "";
#endif
    StringDefaultFlag::Builder builder(MakeValueStore(&device_id_, device_id));

    auto flag = builder.SetLongName("--device_id")
                    .SetHelp("device id for camera")
                    .Build();
    device_id_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }

  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&display_name_, "camera"));

    auto flag = builder.SetLongName("--display_name")
                    .SetHelp("display name for camera")
                    .Build();
    display_name_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
}

CameraFlag::~CameraFlag() = default;

bool CameraFlag::Parse(FlagParser& parser) {
  return NodeCreateFlag::Parse(parser) ||
         PARSE_OPTIONAL_FLAG(parser, device_id_flag_, display_name_flag_);
}

}  // namespace felicia