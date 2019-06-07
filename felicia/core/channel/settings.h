#ifndef FELICIA_CORE_CHANNEL_SETTINGS_H_
#define FELICIA_CORE_CHANNEL_SETTINGS_H_

namespace felicia {
namespace channel {

struct WSSettings {
  WSSettings() = default;

  bool permessage_deflate_enabled = false;
  uint8_t server_max_window_bits = 10;
};

struct Settings {
  Settings() = default;

  WSSettings ws_settings;
};

}  // namespace channel
}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SETTINGS_H_