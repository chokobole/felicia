#ifndef FELICIA_CORE_MASTER_ERRORS_H_
#define FELICIA_CORE_MASTER_ERRORS_H_

#include "felicia/core/lib/error/errors.h"

#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

namespace errors {

inline ::felicia::Status ChannelSourceNotValid(
    const std::string& name, const ChannelSource& channel_source) {
  return InvalidArgument(
      ::base::StringPrintf("Invalid channel source %s: %s.", name.c_str(),
                           channel_source.DebugString().c_str()));
}

inline ::felicia::Status FailedToRegisterClient() {
  return OutOfRange("Failed to register client.");
}

inline ::felicia::Status ClientNotRegistered() {
  return NotFound("Client isn't registered yet.");
}

inline ::felicia::Status NodeNotRegistered(const NodeInfo& node_info) {
  return NotFound(::base::StringPrintf("Node '%s' isn't registered yet.",
                                       node_info.name().c_str()));
}

inline ::felicia::Status NodeAlreadyRegistered(const NodeInfo& node_info) {
  return AlreadyExists(::base::StringPrintf("Node '%s' is already registered.",
                                            node_info.name().c_str()));
}

inline ::felicia::Status TopicAlreadyPublishing(const TopicInfo& topic_info) {
  return AlreadyExists(::base::StringPrintf(
      "Topic '%s' is already being publishied.", topic_info.topic().c_str()));
}

inline ::felicia::Status FailedToPublish(const TopicInfo& topic_info) {
  return Unknown(::base::StringPrintf("Failed to publish topic '%s'.",
                                      topic_info.topic().c_str()));
}

inline ::felicia::Status TopicNotPublishingOnNode(const NodeInfo& node_info,
                                                  const std::string& topic) {
  return NotFound(::base::StringPrintf("Node '%s' isn't publishing topic '%s'.",
                                       node_info.name().c_str(),
                                       topic.c_str()));
}

inline ::felicia::Status FailedToUnpublish(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to unpublish topic '%s'.", topic.c_str()));
}

inline ::felicia::Status TopicAlreadySubscribingOnNode(
    const NodeInfo& node_info, const std::string& topic) {
  return AlreadyExists(
      ::base::StringPrintf("Node '%s' is already subscribing topic '%s'.",
                           node_info.name().c_str(), topic.c_str()));
}

inline ::felicia::Status FailedToSubscribe(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to subscribe topic '%s'.", topic.c_str()));
}

inline ::felicia::Status TopicNotSubscribingOnNode(const NodeInfo& node_info,
                                                   const std::string& topic) {
  return NotFound(
      ::base::StringPrintf("Node '%s' isn't subscribing topic '%s'.",
                           node_info.name().c_str(), topic.c_str()));
}

inline ::felicia::Status FailedToUnsubscribe(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to unsubscribe topic '%s'.", topic.c_str()));
}

}  // namespace errors

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_ERRORS_H_