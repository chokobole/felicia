#ifndef FELICIA_CORE_CHANNEL_SETTINGS_H_
#define FELICIA_CORE_CHANNEL_SETTINGS_H_

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/unit/bytes.h"
#if defined(OS_POSIX)
#include "felicia/core/channel/socket/unix_domain_server_socket.h"
#endif

namespace felicia {
namespace channel {

struct WSSettings {
  WSSettings() = default;

  bool permessage_deflate_enabled = false;
  uint8_t server_max_window_bits = 10;
};

#if defined(OS_POSIX)
struct UDSSettings {
  UDSSettings() = default;

  UnixDomainServerSocket::AuthCallback auth_callback;
};
#endif

struct ShmSettings {
  static constexpr size_t kDefaultShmSize = Bytes::kMegaBytes;

  ShmSettings() = default;

  Bytes shm_size = Bytes::FromBytes(kDefaultShmSize);
};

struct Settings {
  Settings() = default;

  WSSettings ws_settings;
#if defined(OS_POSIX)
  UDSSettings uds_settings;
#endif
  ShmSettings shm_settings;
};

}  // namespace channel
}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SETTINGS_H_