// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/buffered_writer.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

BufferedWriter::BufferedWriter(int option) : option_(option) {}

BufferedWriter::~BufferedWriter() { Close(); }

Status BufferedWriter::Open(const base::FilePath& path) {
  Close();
  file_ =
      base::File(path, base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  if (!file_.IsValid())
    return errors::InvalidArgument(
        base::File::ErrorToString(file_.error_details()));
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity_]);
  return Status::OK();
}

void BufferedWriter::Close() {
  Flush();
  file_.Close();
}

bool BufferedWriter::IsOpened() const { return file_.IsValid(); }

void BufferedWriter::SetBufferCapacityForTesting(size_t buffer_capacity) {
  buffer_capacity_ = buffer_capacity;
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity]);
}

bool BufferedWriter::WriteLine(const std::string& line) {
  size_t to_write = line.length();
  size_t written = 0;
  const char* data = line.c_str();
  int result;
  while (to_write > 0) {
    result = AppendToBufferOrFlush(data + written, to_write);
    if (result < 0) return false;
    written += static_cast<size_t>(result);
    to_write -= static_cast<size_t>(result);
  }
  if (option_ & NEWLINE_CR_LF) {
    result = AppendToBufferOrFlush("\r\n", 2);
  } else {
    result = AppendToBufferOrFlush("\n", 1);
  }
  if (result < 0) return false;
  return true;
}

int BufferedWriter::AppendToBufferOrFlush(const char* data, size_t len) {
  int result = AppendToBuffer(data, std::min(len, capacity()));
  if (capacity() == 0) {
    int result2 = Flush();
    if (result2 < 0) return result2;
  }
  return result;
}

int BufferedWriter::AppendToBuffer(const char* data, size_t len) {
  CHECK_GE(capacity(), len);
  memcpy(buffer_.get() + written_, data, len);
  written_ += len;
  return len;
}

int BufferedWriter::Flush() {
  if (!file_.IsValid()) return -1;
  int result = file_.WriteAtCurrentPos(buffer_.get(), written_);
  // Keep buffer if failed to flush.
  if (result >= 0) written_ = 0;
  return result;
}

}  // namespace felicia