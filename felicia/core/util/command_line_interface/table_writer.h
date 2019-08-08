#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER

#include <string>
#include <vector>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/string_piece.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT TableWriter {
 public:
  TableWriter(const TableWriter& other);
  ~TableWriter();

  struct EXPORT Column {
    Column(std::string title, int width);
    Column(const Column& other);
    ~Column();

    std::string title;
    size_t width = 16;
  };

  void SetElement(size_t row, size_t col, base::StringPiece element);
  std::string ToString() const;

 private:
  friend class TableWriterBuilder;
  TableWriter();

  std::vector<Column> heads_;
  std::vector<std::vector<std::string>> elements_;
};

class EXPORT TableWriterBuilder {
 public:
  TableWriterBuilder();
  ~TableWriterBuilder();

  TableWriterBuilder& AddColumn(const TableWriter::Column& column);

  TableWriter Build() const;

 private:
  TableWriter writer_;

  DISALLOW_COPY_AND_ASSIGN(TableWriterBuilder);
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER