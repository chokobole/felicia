// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_
#define FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

// This class is used from other than c++ side, when from the c++ side,
// it doens't know how to serialize or deserialize the message.
class FEL_EXPORT SerializedMessage {
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

FEL_EXPORT std::ostream& operator<<(std::ostream& os,
                                    const SerializedMessage& m);

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_SERIALIZED_MESSAGE_H_