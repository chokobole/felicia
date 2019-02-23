#include "felicia/core/util/command_line_interface/table_writer.h"

#include <iomanip>
#include <sstream>

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

TableWriter::TableWriter() = default;

TableWriterBuilder::TableWriterBuilder() = default;

TableWriterBuilder& TableWriterBuilder::AddColumn(
    const TableWriter::Column& column) {
  writer_.heads_.push_back(column);
  return *this;
}

void TableWriter::SetElement(size_t row, size_t col,
                             ::base::StringPiece element) {
  if (elements_.size() <= row) {
    elements_.resize(row + 1);
  }
  if (elements_[row].size() <= col) {
    elements_[row].resize(heads_.size());
  }

  elements_[row][col] = std::string(element);
}

std::string TableWriter::ToString() const {
  std::stringstream ss;
  for (auto& head : heads_) {
    ss << std::left << std::setw(head.width) << head.title;
  }
  ss << std::endl;

  for (auto& row : elements_) {
    size_t idx = 0;
    for (auto& element : row) {
      ss << std::left << std::setw(heads_[idx].width) << element;
      idx++;
    }
    ss << std::endl;
  }

  return ss.str();
}

}  // namespace felicia