#if defined(HAS_ROS)

#ifndef FELICIA_CORE_MESSAGE_ROS_RPC_HEADER_H_
#define FELICIA_CORE_MESSAGE_ROS_RPC_HEADER_H_

#include "felicia/core/message/header.h"

namespace felicia {

class EXPORT RosRpcHeader : public Header {
 public:
  RosRpcHeader();
  ~RosRpcHeader();

  MessageIOError AttachHeader(const std::string& content, std::string* text);
  int header_size() const;
  MessageIOError ParseHeader(const char* buffer, int* mesasge_offset,
                             int* message_size);

  bool ok() const;
  void set_ok(bool ok);

 private:
  bool ok_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_ROS_RPC_HEADER_H_

#endif  // defined(HAS_ROS)