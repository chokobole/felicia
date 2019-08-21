#ifndef FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_
#define FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_

#include <memory>

#include "third_party/chromium/base/files/file.h"
#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class EXPORT BufferedReader {
 public:
  BufferedReader();

  Status Open(const base::FilePath& path);

  void SetBufferCapacityForTesting(size_t buffer_capacity);

  bool ReadLine(std::string* line);

  bool eof() const;

 private:
  void ReadInAdvance();

  static constexpr size_t kDefaultBufferCapacity = 256;

  base::File file_;
  size_t buffer_capacity_ = kDefaultBufferCapacity;
  size_t buffer_size_ = 0;
  size_t read_ = 0;
  std::unique_ptr<char[]> buffer_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_BUFFERED_READER_H_