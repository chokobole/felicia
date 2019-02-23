#include "felicia/core/util/command_line_interface/flag.h"

namespace felicia {

EXPORT std::string MakeNamedHelpText(::base::StringPiece name,
                                     ::base::StringPiece help, int help_start) {
  int remain_len = help_start;
  std::stringstream ss;
  APPEND_AND_DECREASE_LENGTH(ss, name, remain_len);
  ALIGN_AT_INDEX_AND_APPEND(ss, help, remain_len, help_start);
}

}  // namespace felicia