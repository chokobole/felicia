#ifndef FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_
#define FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

// This class is used from other than c++ side, when from the c++ side,
// it doens't know how to serialize or deserialize the message.
class EXPORT SerializedMessage {
 public:
  SerializedMessage();
  SerializedMessage(const SerializedMessage& other);
  SerializedMessage& operator=(const SerializedMessage& other);
  SerializedMessage(SerializedMessage&& other) noexcept;
  SerializedMessage& operator=(SerializedMessage&& other) noexcept;
  ~SerializedMessage();

  void set_serialized(const std::string& serialized);
  void set_serialized(std::string&& serialized);

  const std::string& serialized() const&;
  std::string&& serialized() &&;

  bool SerializeToString(std::string* text) const&;
  bool SerializeToString(std::string* text) &&;

 private:
  std::string serialized_;
};

EXPORT std::ostream& operator<<(std::ostream& os, const SerializedMessage& m);

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_