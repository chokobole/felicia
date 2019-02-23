#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TEXT_STYLE_H_
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TEXT_STYLE_H_

#include "third_party/chromium/base/strings/strcat.h"

#define RED_STYLE "\033[91m"
#define GREEN_STYLE "\033[32m"
#define BLUE_STYLE "\033[34m"
#define YELLOW_STYLE "\033[93m"
#define BOLD_STYLE "\033[1m"
#define NONE_STYLE "\033[0m"

#define APPLY_STYLE(style, text) ::base::StrCat({style, text, NONE_STYLE})

#define RED_COLORED(text) APPLY_STYLE(RED_STYLE, text)
#define GREEN_COLORED(text) APPLY_STYLE(GREEN_STYLE, text)
#define BLUE_COLORED(text) APPLY_STYLE(BLUE_STYLE, text)
#define YELLOW_COLORED(text) APPLY_STYLE(YELLOW_STYLE, text)
#define BOLD(text) APPLY_STYLE(BOLD_STYLE, text)

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_TEXT_STYLE_H_
