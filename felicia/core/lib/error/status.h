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

#ifndef FELICIA_CORE_LIB_ERROR_STATUS_H_
#define FELICIA_CORE_LIB_ERROR_STATUS_H_

#include <string>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/strings/string_piece.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/error_codes.pb.h"

namespace felicia {

// Denotes success or failure of a call in Felicia.
class EXPORT Status {
 public:
  // Create a success status.
  Status();
  Status(felicia::error::Code error_code, ::base::StringPiece error_message);

  // Convenience static method.
  static Status OK();

  Status(const Status& status);
  Status& operator=(const Status& status);
  ~Status();

  // Accessor
  error::Code error_code() const;
  const std::string& error_message() const;
  bool ok() const;

  bool operator==(const Status& status) const;
  bool operator!=(const Status& status) const;

 private:
  felicia::error::Code error_code_;
  std::string error_message_;
};

// Prints a human-readable representation of |status| to |os|.
EXPORT std::ostream& operator<<(std::ostream& os, const Status& x);

// Convenient typedef for a closure passing a Status.
typedef ::base::OnceCallback<void(const Status&)> StatusCallback;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_ERROR_STATUS_H_