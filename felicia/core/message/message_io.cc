#include "felicia/core/message/message_io.h"

namespace felicia {

std::string MessageIOErrorToString(MessageIOError mesasge_io_error) {
  switch (mesasge_io_error) {
#define MESSAGE_IO_ERR(ERR, ERR_STR) \
  case MessageIOError::ERR:          \
    return ERR_STR;
#include "felicia/core/message/message_io_error_list.h"
#undef MESSAGE_IO_ERR
  }
  NOTREACHED();
}

// static
MessageIOError MessageIO::ParseHeaderFromBuffer(const char* buffer,
                                                Header* header,
                                                bool receive_from_ros) {
  if (!Header::FromBytes(buffer, header, receive_from_ros))
    return MessageIOError::ERR_CORRUPTED_HEADER;

  return MessageIOError::OK;
}

}  // namespace felicia