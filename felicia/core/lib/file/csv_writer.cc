#include "felicia/core/lib/file/csv_writer.h"

#include "third_party/chromium/base/strings/string_util.h"

namespace felicia {

CsvWriter::CsvWriter(int option) : writer_(option) {}

Status CsvWriter::Open(const base::FilePath& path,
                       const std::string& delimiter) {
  writer_.Close();
  delimiter_ = delimiter;
  Status s = writer_.Open(path);
  if (!s.ok()) return s;
  return Status::OK();
}

void CsvWriter::Close() { writer_.Close(); }

bool CsvWriter::IsOpened() const { return writer_.IsOpened(); }

bool CsvWriter::WriteRows(const std::vector<std::string>& rows) {
  std::string text = base::JoinString(rows, delimiter_);
  return writer_.WriteLine(text);
}

}  // namespace felicia