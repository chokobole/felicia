#ifndef FELICIA_CORE_LIB_FILE_BUFFERED_WRITER_H_
#define FELICIA_CORE_LIB_FILE_BUFFERED_WRITER_H_

#include <memory>

#include "third_party/chromium/base/files/file.h"
#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT BufferedWriter {
 public:
  enum Option {
    NONE,  // default add linefeed.
    NEWLINE_CR_LF,
  };

  explicit BufferedWriter(int option = Option::NONE);
  ~BufferedWriter();

  void set_option(int option) { option_ = option; }

  Status Open(const base::FilePath& path);

  void Close();

  bool IsOpened() const;

  void SetBufferCapacityForTesting(size_t buffer_capacity);

  bool WriteLine(const std::string& line);

  std::string buffer() const { return std::string(buffer_.get(), written_); }

 private:
  size_t capacity() const { return buffer_capacity_ - written_; }

  int AppendToBufferOrFlush(const char* data, size_t len);
  int AppendToBuffer(const char* data, size_t len);
  int Flush();

  static constexpr size_t kDefaultBufferCapacity = 256;

  base::File file_;
  size_t buffer_capacity_ = kDefaultBufferCapacity;
  size_t written_ = 0;
  int option_;
  std::unique_ptr<char[]> buffer_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_BUFFERED_WRITER_H_