#ifndef FELICIA_CORE_MESSAGE_HEADER_H_
#define FELICIA_CORE_MESSAGE_HEADER_H_

#include <stdint.h>
#include <string>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/message_io.h"

namespace felicia {

class FEL_EXPORT Header {
 public:
  Header();
  ~Header();

  // Needed by MessageSender<T>
  MessageIOError AttachHeader(const std::string& content, std::string* text);
  // Needed by MessageReceiver<T>
  int header_size() const;
  MessageIOError ParseHeader(const char* buffer, int* mesasge_offset,
                             int* message_size);

  MessageIOError AttachHeaderInternally(const std::string& content,
                                        char* buffer);

  int size() const;
  void set_size(int size);

 protected:
  int size_ = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_HEADER_H_