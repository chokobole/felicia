#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

#include <iostream>

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/unit/bytes.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

CameraFlag::CameraFlag() {
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
}

CameraFlag::~CameraFlag() = default;

bool CameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, is_publishing_node_flag_, name_flag_,
                             topic_flag_, device_list_flag_,
                             device_index_flag_);
}

bool CameraFlag::Validate() const {
  if (device_list_flag_->is_set()) {
    PrintDeviceListFlagHelp();
    return true;
  }

  if (!NodeCreateFlag::Validate()) return false;

  if (is_publishing_node_) {
    return CheckIfFlagWasSet(device_index_flag_);
  }

  return true;
}

void CameraFlag::PrintDeviceListFlagHelp() const {
  std::cout << "device_list is on, it just shows a list of camera devices. "
               "If you pass -i(--device_index) with the -l then you can "
               "iterate the camera formats the device supports."
            << std::endl;
}

}  // namespace felicia