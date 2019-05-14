#ifndef FELICIA_CORE_COMMUNICATION_SETTINGS_H_
#define FELICIA_CORE_COMMUNICATION_SETTINGS_H_

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {
namespace communication {

struct Settings {
  static constexpr int64_t kDefaultPeriod = 1000;
  static constexpr size_t kDefaultMessageSize = Bytes::kMegaBytes;
  static constexpr uint8_t kDefaultQueueSize = 100;

  Settings() {}
  Settings(::base::TimeDelta period, Bytes buffer_size, bool is_dynamic_buffer,
           uint8_t queue_size)
      : period(period),
        buffer_size(buffer_size),
        is_dynamic_buffer(is_dynamic_buffer),
        queue_size(queue_size) {}

  ::base::TimeDelta period = ::base::TimeDelta::FromMilliseconds(
      kDefaultPeriod);  // used only in subscriber
  Bytes buffer_size = Bytes::FromBytes(kDefaultMessageSize);
  bool is_dynamic_buffer = false;
  uint8_t queue_size = kDefaultQueueSize;
};

}  // namespace communication
}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SETTINGS_H_