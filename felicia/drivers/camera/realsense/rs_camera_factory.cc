#include "felicia/drivers/camera/realsense/rs_camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/drivers/camera/realsense/rs_pixel_format.h"

namespace felicia {

constexpr const char* kStereoModule = "Stereo Module";
constexpr const char* kRGBModule = "RGB Camera";
constexpr const char* kMotionModule = "Motion Module";

// static
std::unique_ptr<DepthCameraInterface> RsCameraFactory::NewDepthCamera(
    const CameraDescriptor& descriptor) {
  return ::base::WrapUnique(new RsCamera(descriptor));
}

// static
Status RsCameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors->empty());
  ::rs2::context context;
  ::rs2::device_list list = context.query_devices();
  for (auto&& dev : list) {
    const char* device_name = dev.get_info(RS2_CAMERA_INFO_NAME);
    const char* physical_port = dev.get_info(RS2_CAMERA_INFO_PHYSICAL_PORT);
    const char* product_id = dev.get_info(RS2_CAMERA_INFO_PRODUCT_ID);

    camera_descriptors->emplace_back(device_name, physical_port, product_id);
  }

  return Status::OK();
}

// static
Status RsCameraFactory::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor,
    RsCameraFormatMap* rs_camera_format_map) {
  DCHECK(rs_camera_format_map->empty());
  ::rs2::device device;
  Status s = CreateDevice(camera_descriptor, &device);
  if (!s.ok()) return s;

  std::vector<::rs2::sensor> sensors = device.query_sensors();
  for (auto&& sensor : sensors) {
    // std::string module_name = sensor.get_info(RS2_CAMERA_INFO_NAME);
    std::vector<::rs2::stream_profile> profiles = sensor.get_stream_profiles();
    for (auto& profile : profiles) {
      if (profile.is<::rs2::video_stream_profile>()) {
        auto video_profile = profile.as<rs2::video_stream_profile>();
        RsStreamInfo steram_info{video_profile.stream_type(),
                                 video_profile.stream_index()};
        if (rs_camera_format_map->find(steram_info) ==
            rs_camera_format_map->end()) {
          (*rs_camera_format_map)[steram_info] = CameraFormats{};
        }

        (*rs_camera_format_map)[steram_info].emplace_back(
            video_profile.width(), video_profile.height(),
            FromRs2Format(video_profile.format()), video_profile.fps());
      }
    }
  }

  return Status::OK();
}

// static
Status RsCameraFactory::CreateDevice(const CameraDescriptor& camera_descriptor,
                                     ::rs2::device* device) {
  ::rs2::context context;
  auto list = context.query_devices();
  for (auto&& dev : list) {
    const char* physical_port = dev.get_info(RS2_CAMERA_INFO_PHYSICAL_PORT);
    if (physical_port == camera_descriptor.device_id()) {
      *device = dev;
      return Status::OK();
    }
  }

  return Status(error::NOT_FOUND,
                ::base::StrCat({"No mathcing device with ",
                                camera_descriptor.ToString()}));
}

}  // namespace felicia