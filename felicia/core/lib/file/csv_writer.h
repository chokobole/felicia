#ifndef FELICIA_CORE_LIB_FILE_CSV_WRITER_H_
#define FELICIA_CORE_LIB_FILE_CSV_WRITER_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_writer.h"

namespace felicia {

class FEL_EXPORT CsvWriter {
 public:
  explicit CsvWriter(int option = BufferedWriter::NONE);

  Status Open(const base::FilePath& path, const std::string& delimiter = ",");

  void Close();

  bool IsOpened() const;

  // Return true if writes succeeds.
  bool WriteRows(const std::vector<std::string>& rows);

 private:
  BufferedWriter writer_;
  std::string delimiter_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_CSV_WRITER_H_