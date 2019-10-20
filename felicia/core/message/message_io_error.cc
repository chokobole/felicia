#include "felicia/core/message/message_io_error.h"

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

}  // namespace felicia