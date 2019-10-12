#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

Bytes kHeartBeatBytes = Bytes::FromBytes(128);
Bytes kMasterNotificationBytes = Bytes::FromBytes(256);

}  // namespace felicia