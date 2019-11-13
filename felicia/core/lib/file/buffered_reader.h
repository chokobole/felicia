// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_
#define FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_

#include <memory>

#include "third_party/chromium/base/files/file.h"
#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT BufferedReader {
 public:
  enum Option {
    NONE,
    REMOVE_CR_OR_LF,
  };

  explicit BufferedReader(int option = Option::NONE);

  void set_option(int option) { option_ = option; }

  Status Open(const base::FilePath& path);

  void Close();

  bool IsOpened() const;

  void SetBufferCapacityForTesting(size_t buffer_capacity);

  // Return true if read succeeds, return false when eof or
  // |line| is not empty. We expect |line| to be empty.
  // If you just want to skip line, you can set |line| to nullptr.
  bool ReadLine(std::string* line = nullptr);

  bool eof() const;

 private:
  void ReadInAdvance();

  static constexpr size_t kDefaultBufferCapacity = 256;

  base::File file_;
  size_t buffer_capacity_ = kDefaultBufferCapacity;
  size_t buffer_size_ = 0;
  size_t read_ = 0;
  int option_;
  std::unique_ptr<char[]> buffer_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_