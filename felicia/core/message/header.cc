#include "felicia/core/message/header.h"

#include <string.h>

namespace felicia {

Header::Header() = default;

Header::~Header() = default;

MessageIOError Header::AttachHeader(const std::string& content,
                                    std::string* text) {
  size_ = content.length();
  text->resize(header_size() + size_);
  char* buffer = const_cast<char*>(text->c_str());
  memcpy(buffer, &size_, sizeof(int));
  buffer += sizeof(int);
  memcpy(buffer, content.c_str(), size_);
  return MessageIOError::OK;
}

int Header::header_size() const { return sizeof(int); }

MessageIOError Header::ParseHeader(const char* buffer, int* mesasge_offset,
                                   int* message_size) {
  size_ = *reinterpret_cast<const int*>(buffer);
  *message_size = size_;
  *mesasge_offset = header_size();
  return MessageIOError::OK;
}

MessageIOError Header::AttachHeaderInternally(const std::string& content,
                                              char* buffer) {
  size_ = content.length();
  memcpy(buffer, &size_, sizeof(int));
  buffer += sizeof(int);
  memcpy(buffer, content.c_str(), size_);
  return MessageIOError::OK;
}

int Header::size() const { return size_; }

void Header::set_size(int size) { size_ = size; }

}  // namespace felicia