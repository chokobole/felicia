#include "felicia/core/channel/socket/web_socket_extension.h"

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/channel/socket/permessage_deflate.h"

namespace felicia {

WebSocketExtensionInterface::WebSocketExtensionInterface() = default;
WebSocketExtensionInterface::~WebSocketExtensionInterface() = default;

WebSocketExtension::WebSocketExtension() {
  extensions_[PermessageDeflate::kKey] = std::make_unique<PermessageDeflate>();
}

bool WebSocketExtension::Negotiate(const std::string& extension,
                                   std::string* response) {
  ::base::StringTokenizer params(extension.cbegin(), extension.cend(), ";");

  if (!params.GetNext()) return false;

  ::base::StringPiece key = ::base::TrimWhitespaceASCII(
      params.token_piece(), ::base::TrimPositions::TRIM_ALL);

  auto it = extensions_.find(key);
  if (it == extensions_.end()) {
    DLOG(ERROR) << "No extension for " << key;
    return false;
  }

  return it->second->Negotiate(params, response);
}

}  // namespace felicia