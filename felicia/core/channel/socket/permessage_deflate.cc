#include "felicia/core/channel/socket/permessage_deflate.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

constexpr const char* kClientNoContextTakeover = "client_no_context_takeover";
constexpr const char* kServerNoContextTakeover = "server_no_context_takeover";
constexpr const char* kClientMaxWindowBits = "client_max_window_bits";
constexpr const char* kServerMaxWindowBits = "server_max_window_bits";
constexpr const int kMinWindowBits = 8;
constexpr const int kMaxWindowBits = 15;

const char* PermessageDeflate::kKey = "permessage-deflate";

#define CHECK_NO_VALUE(key, value)                        \
  if (!value.empty()) {                                   \
    DLOG(ERROR) << "Should not have a value for " << key; \
    return false;                                         \
  }

#define VALIDATE_IF_HAVE_VALUE(key, value, out_value)                        \
  if (!value.empty()) {                                                      \
    if (::base::StringToInt(value, &out_value)) {                            \
      if (out_value < kMinWindowBits || out_value > kMaxWindowBits) {        \
        DLOG(ERROR) << "Value should be between " << kMinWindowBits << " ~ " \
                    << kMaxWindowBits << " for " << key << " but got "       \
                    << out_value;                                            \
        return false;                                                        \
      }                                                                      \
    } else {                                                                 \
      return false;                                                          \
    }                                                                        \
  }

bool PermessageDeflate::Negotiate(::base::StringTokenizer& params,
                                  std::string* response) {
  bool client_no_context_takeover = true;
  bool server_no_context_takeover = true;
  int client_max_window_bits = kMaxWindowBits;
  int server_max_window_bits = kMaxWindowBits;

  while (params.GetNext()) {
    std::string::const_iterator param_begin = params.token_begin();
    std::string::const_iterator param_end = params.token_end();

    std::string::const_iterator end = std::find(param_begin, param_end, '=');
    ::base::StringPiece key = ::base::TrimWhitespaceASCII(
        ::base::StringPiece(&*param_begin, std::distance(param_begin, end)),
        ::base::TrimPositions::TRIM_ALL);
    ::base::StringPiece value = ::base::EmptyString();
    if (param_end != end) {
      value = ::base::TrimWhitespaceASCII(
          ::base::StringPiece(&*(end + 1), std::distance(end + 1, param_end)),
          ::base::TrimPositions::TRIM_ALL);

      if (strings::ConsumePrefix(&value, "\"")) {
        if (!strings::ConsumeSuffix(&value, "\"")) {
          DLOG(ERROR) << "value not end with \"" << value;
          return false;
        }
      }
    }

    // Ignore the request currently, just validate the params.
    // TODO: Implement negotiation
    if (key == kClientNoContextTakeover) {
      CHECK_NO_VALUE(key, value);
    } else if (key == kServerNoContextTakeover) {
      CHECK_NO_VALUE(key, value);
    } else if (key == kClientMaxWindowBits) {
      int temp_value;  // not used
      VALIDATE_IF_HAVE_VALUE(key, value, temp_value);
    } else if (key == kServerMaxWindowBits) {
      int temp_value;  // not used
      VALIDATE_IF_HAVE_VALUE(key, value, temp_value);
    } else {
      DLOG(ERROR) << "Unknown param " << key;
      return false;
    }
  }

  ::base::StringAppendF(response, "%s", kKey);
  if (client_no_context_takeover) {
    ::base::StringAppendF(response, "; %s", kClientNoContextTakeover);
  }
  if (server_no_context_takeover) {
    ::base::StringAppendF(response, "; %s", kServerNoContextTakeover);
  }
  ::base::StringAppendF(response, "; %s=%d", kServerMaxWindowBits,
                        server_max_window_bits);

  return true;
}

#undef CHECK_NO_VALUE
#undef VALIDATE_IF_HAVE_VALUE

}  // namespace felicia