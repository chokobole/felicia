#include "felicia/drivers/camera/camera_settings.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

#define DEFINE_NAMED_VALUE_METHOD(type, name)                             \
  bool CameraSettings::has_##name() const { return name##_.has_value(); } \
                                                                          \
  void CameraSettings::set_##name(type name) { name##_ = name; }          \
  type CameraSettings::name() const {                                     \
    DCHECK(name##_.has_value());                                          \
    return name##_.value();                                               \
  }

DEFINE_NAMED_VALUE_METHOD(CameraSettingsMode, white_balance_mode)
DEFINE_NAMED_VALUE_METHOD(CameraSettingsMode, exposure_mode)
DEFINE_NAMED_VALUE_METHOD(int64_t, exposure_compensation)
DEFINE_NAMED_VALUE_METHOD(int64_t, exposure_time)
DEFINE_NAMED_VALUE_METHOD(int64_t, color_temperature)
DEFINE_NAMED_VALUE_METHOD(int64_t, brightness)
DEFINE_NAMED_VALUE_METHOD(int64_t, contrast)
DEFINE_NAMED_VALUE_METHOD(int64_t, saturation)
DEFINE_NAMED_VALUE_METHOD(int64_t, sharpness)

#undef DEFINE_NAMED_VALUE_METHOD

}  // namespace felicia