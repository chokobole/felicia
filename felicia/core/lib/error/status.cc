/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "felicia/core/lib/error/status.h"

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

Status::Status() : error_code_(error::Code::OK) {}

Status::Status(felicia::error::Code error_code,
               ::base::StringPiece error_message)
    : error_code_(error_code), error_message_(std::string(error_message)) {}

Status::Status(const Status& status) = default;
Status& Status::operator=(const Status& status) = default;

Status::~Status() = default;

// static
Status Status::OK() { return Status(); }

error::Code Status::error_code() const { return error_code_; }
const std::string& Status::error_message() const { return error_message_; }
bool Status::ok() const { return error_code_ == 0; }

bool Status::operator==(const Status& status) const {
  return error_code_ == status.error_code_ &&
         strings::Equals(error_message_, status.error_message_);
}

bool Status::operator!=(const Status& status) const {
  return !operator==(status);
}

std::ostream& operator<<(std::ostream& os, const Status& status) {
  os << status.error_message();
  return os;
}

}  // namespace felicia