#include "felicia/core/master/bytes_constants.h"

namespace felicia {

Bytes kHeartBeatBytes = Bytes::FromBytes(128);
Bytes kMasterNotificationBytes = Bytes::FromBytes(256);

}  // namespace felicia