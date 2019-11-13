// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/buffered_reader.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

BufferedReader::BufferedReader(int option) : option_(option) {}

Status BufferedReader::Open(const base::FilePath& path) {
  file_.Close();
  file_ = base::File(path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!file_.IsValid())
    return errors::InvalidArgument(
        base::File::ErrorToString(file_.error_details()));
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity_]);
  ReadInAdvance();
  return Status::OK();
}

void BufferedReader::Close() { file_.Close(); }

bool BufferedReader::IsOpened() const { return file_.IsValid(); }

void BufferedReader::SetBufferCapacityForTesting(size_t buffer_capacity) {
  buffer_capacity_ = buffer_capacity;
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity]);
}

bool BufferedReader::ReadLine(std::string* line) {
  if (line && !line->empty()) return false;
  if (eof()) return false;

  bool met_linefeed = false;
  while (!met_linefeed) {
    size_t begin = read_;
    while (read_ < buffer_size_) {
      char c = buffer_[read_++];
      if (c == '\n') {
        met_linefeed = true;
        break;
      }
    }
    size_t len = read_ - begin;
    if (met_linefeed && (option_ & REMOVE_CR_OR_LF)) {
      if (len >= 2 && buffer_[read_ - 2] == '\r') {
        len -= 2;
      } else {
        len -= 1;
      }
      if (line && line->length() > 0 && (*line)[line->length() - 1] == '\r') {
        *line = line->substr(0, line->length() - 1);
      }
    }
    if (line) line->append(buffer_.get() + begin, len);
    if (!met_linefeed || eof()) {
      ReadInAdvance();
      if (eof()) break;
    }
  }
  return true;
}

bool BufferedReader::eof() const {
  CHECK(buffer_);
  return buffer_size_ == read_;
}

void BufferedReader::ReadInAdvance() {
  read_ = 0;
  buffer_size_ = file_.ReadAtCurrentPos(buffer_.get(), buffer_capacity_);
}

}  // namespace felicia