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

#define CHECK_NO_VALUE(key, value)                        \
  if (!value.empty()) {                                   \
    DLOG(ERROR) << "Should not have a value for " << key; \
    return false;                                         \
  }

#define VALIDATE_IF_HAVE_VALUE(key, value, out_value)                        \
  if (!value.empty()) {                                                      \
    if (::base::StringToInt(value, reinterpret_cast<int*>(&out_value))) {    \
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
                                  const channel::WSSettings& settings,
                                  std::string* response) {
  DCHECK(settings.permessage_deflate_enabled);
  client_no_context_takeover_ = true;
  server_no_context_takeover_ = true;
  client_max_window_bits_ = kMaxWindowBits;
  server_max_window_bits_ = settings.server_max_window_bits;

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
      VALIDATE_IF_HAVE_VALUE(key, value, client_max_window_bits_);
    } else if (key == kServerMaxWindowBits) {
      int temp_value;  // not used
      VALIDATE_IF_HAVE_VALUE(key, value, temp_value);
    } else {
      DLOG(ERROR) << "Unknown param " << key;
      return false;
    }
  }

  AppendResponse(response);
  return true;
}

void PermessageDeflate::AppendResponse(std::string* response) const {
  ::base::StringAppendF(response, "%s", kKey);
  if (client_no_context_takeover_) {
    ::base::StringAppendF(response, "; %s", kClientNoContextTakeover);
  }
  if (server_no_context_takeover_) {
    ::base::StringAppendF(response, "; %s", kServerNoContextTakeover);
  }
  ::base::StringAppendF(response, "; %s=%d", kServerMaxWindowBits,
                        server_max_window_bits_);
}

#undef CHECK_NO_VALUE
#undef VALIDATE_IF_HAVE_VALUE

}  // namespace felicia