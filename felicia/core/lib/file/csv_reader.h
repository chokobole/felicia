#ifndef FELICIA_CORE_LIB_FILE_CSV_READER_H_
#define FELICIA_CORE_LIB_FILE_CSV_READER_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"

namespace felicia {

class EXPORT CsvReader {
 public:
  CsvReader();

  Status Open(const base::FilePath& path, const std::string& delimiter = ",",
              int skip_head = 0);

  void Close();

  bool IsOpened() const;

  // Return true if read succeeds, return false when eof or
  // |rows| is not empty. We expect |rows| to be empty.
  bool ReadRows(std::vector<std::string>* rows);

  bool eof() const;

 private:
  BufferedReader reader_;
  std::string delimiter_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_CSV_READER_H_