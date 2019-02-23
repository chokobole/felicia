#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER

#include <string>
#include <vector>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/string_piece.h"

namespace felicia {

class TableWriter {
 public:
  struct Column {
    Column(std::string title, int width) : title(title), width(width) {}

    std::string title;
    size_t width = 16;
  };

  void SetElement(size_t row, size_t col, ::base::StringPiece element);
  std::string ToString() const;

 private:
  friend class TableWriterBuilder;
  TableWriter();

  std::vector<Column> heads_;
  std::vector<std::vector<std::string>> elements_;
};

class TableWriterBuilder {
 public:
  TableWriterBuilder();

  TableWriterBuilder& AddColumn(const TableWriter::Column& column);

  TableWriter Build() const { return writer_; }

 private:
  TableWriter writer_;

  DISALLOW_COPY_AND_ASSIGN(TableWriterBuilder);
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TABLE_WRITER