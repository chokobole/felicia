#ifndef FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAMERA_H_
#define FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAMERA_H_

#include <atomic>

#include <librealsense2/rs.hpp>

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/drivers/camera/depth_camera_interface.h"
#include "felicia/drivers/imu/imu.h"
#include "felicia/drivers/vendors/realsense/rs_capability.h"

namespace felicia {

class PipelineSyncer : public ::rs2::asynchronous_syncer {
 public:
  void operator()(::rs2::frame f) const { invoke(std::move(f)); }
};

class RsCamera : public DepthCameraInterface {
 public:
  ~RsCamera();

  // DepthCameraInterface methods
  Status Init() override;
  Status Start(const CameraFormat& requested_color_format,
               const CameraFormat& requested_depth_format,
               CameraFrameCallback color_frame_callback,
               CameraFrameCallback depth_frame_callback,
               StatusCallback status_callback) override;
  Status Start(const CameraFormat& requested_color_format,
               const CameraFormat& requested_depth_format,
               DepthCameraFrameCallback depth_camera_frame_callback,
               StatusCallback status_callback) override;
  Status Stop();

  Status Start(const CameraFormat& requested_color_format,
               const CameraFormat& requested_depth_format,
               const ImuFormat& requested_gyro_format,
               const ImuFormat& requested_accel_format,
               CameraFrameCallback color_frame_callback,
               CameraFrameCallback depth_frame_callback,
               ImuCallback imu_callback, StatusCallback status_callback);
  Status Start(const CameraFormat& requested_color_format,
               const CameraFormat& requested_depth_format,
               const ImuFormat& requested_gyro_format,
               const ImuFormat& requested_accel_format,
               DepthCameraFrameCallback depth_camera_frame_callback,
               ImuCallback imu_callback, StatusCallback status_callback);

 private:
  friend class RsCameraFactory;

  RsCamera(const CameraDescriptor& camera_descriptor);

  Status Start(const CameraFormat& requested_color_format,
               const CameraFormat& requested_depth_format,
               const ImuFormat& requested_gyro_format,
               const ImuFormat& requested_accel_format, bool imu, bool synched);

  void OnFrame(::rs2::frame frame);
  void OnImu(::rs2::frame frame);

  void SetFirstRefTime();

  ::base::Optional<CameraFrame> FromRsColorFrame(
      ::rs2::video_frame color_frame);
  CameraFrame FromRsDepthFrame(::rs2::depth_frame depth_frame);

  static Status CreateDevice(const CameraDescriptor& camera_descriptor,
                             ::rs2::device* device);

  static Status CreateCapabilityMap(::rs2::device device,
                                    RsCapabilityMap* rs_capability_map);

  ::rs2::device device_;
  PipelineSyncer syncer_;

  ::base::flat_map<RsStreamInfo, ::rs2::sensor> sensors_;
  RsCapabilityMap capability_map_;

  ImuFormat gyro_format_;
  ImuFormat accel_format_;
  ImuCallback imu_callback_;

  ThreadSafeTimestamper timestamper_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(RsCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAMERA_H_