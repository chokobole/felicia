#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_SETTINGS_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_SETTINGS_H_

#include "third_party/chromium/base/optional.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/camera/camera_settings_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT CameraSettings {
 public:
#define DECLARE_NAMED_VALUE_METHOD(type, name) \
  bool has_##name() const;                     \
  void set_##name(type name);                  \
  type name() const

  DECLARE_NAMED_VALUE_METHOD(CameraSettingsMode, white_balance_mode);
  DECLARE_NAMED_VALUE_METHOD(CameraSettingsMode, exposure_mode);
  DECLARE_NAMED_VALUE_METHOD(int64_t, exposure_compensation);
  DECLARE_NAMED_VALUE_METHOD(int64_t, exposure_time);
  DECLARE_NAMED_VALUE_METHOD(int64_t, color_temperature);
  DECLARE_NAMED_VALUE_METHOD(int64_t, brightness);
  DECLARE_NAMED_VALUE_METHOD(int64_t, contrast);
  DECLARE_NAMED_VALUE_METHOD(int64_t, saturation);
  DECLARE_NAMED_VALUE_METHOD(int64_t, sharpness);
  DECLARE_NAMED_VALUE_METHOD(int64_t, hue);
  DECLARE_NAMED_VALUE_METHOD(int64_t, gain);
  DECLARE_NAMED_VALUE_METHOD(int64_t, gamma);

#undef DECLARE_NAMED_VALUE_METHOD

 private:
  base::Optional<CameraSettingsMode> white_balance_mode_;
  base::Optional<CameraSettingsMode> exposure_mode_;
  base::Optional<int64_t> exposure_compensation_;
  base::Optional<int64_t> exposure_time_;
  base::Optional<int64_t> color_temperature_;
  base::Optional<int64_t> brightness_;
  base::Optional<int64_t> contrast_;
  base::Optional<int64_t> saturation_;
  base::Optional<int64_t> sharpness_;
  base::Optional<int64_t> hue_;
  base::Optional<int64_t> gain_;
  base::Optional<int64_t> gamma_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_SETTINGS_H_