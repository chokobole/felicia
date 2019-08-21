#include "felicia/core/lib/file/buffered_reader.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

BufferedReader::BufferedReader() = default;

Status BufferedReader::Open(const base::FilePath& path) {
  file_ = base::File(path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!file_.IsValid())
    return errors::InvalidArgument(
        base::File::ErrorToString(file_.error_details()));
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity_]);
  ReadInAdvance();
  return Status::OK();
}

void BufferedReader::SetBufferCapacityForTesting(size_t buffer_capacity) {
  buffer_capacity_ = buffer_capacity;
  buffer_ = std::unique_ptr<char[]>(new char[buffer_capacity]);
}

bool BufferedReader::ReadLine(std::string* line) {
  if (eof()) return false;

  bool met_linefeed = false;
  while (!met_linefeed) {
    size_t begin = read_;
    while (read_ < buffer_size_) {
      if (buffer_[read_++] == '\n') {
        met_linefeed = true;
        break;
      }
    }
    line->append(buffer_.get() + begin, read_ - begin);
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