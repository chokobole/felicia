#include "felicia/examples/learn/message_communication/camera/cc/camera_flag.h"

#include <iostream>

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/unit/bytes.h"
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
    DefaultFlag<size_t>::Builder builder(
        MakeValueStore(&buffer_size_, Bytes::kMegaBytes * 5));

    auto flag = builder.SetLongName("--buffer_size")
                    .SetHelp("buffer size for each frame")
                    .Build();
    buffer_size_flag_ = std::make_unique<DefaultFlag<size_t>>(flag);
  }
}

CameraFlag::~CameraFlag() = default;

bool CameraFlag::Parse(FlagParser& parser) {
  return NodeCreateFlag::Parse(parser) ||
         PARSE_OPTIONAL_FLAG(parser, device_id_flag_, buffer_size_flag_);
}

}  // namespace felicia