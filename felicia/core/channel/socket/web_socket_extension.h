#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_EXTENSION_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_EXTENSION_H_

#include <memory>
#include <string>

#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/strings/string_tokenizer.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT WebSocketExtensionInterface {
 public:
  WebSocketExtensionInterface();
  virtual ~WebSocketExtensionInterface();
  virtual bool Negotiate(::base::StringTokenizer& params,
                         std::string* response) = 0;
  virtual void AppendResponse(std::string* response) const = 0;
};

class WebSocketExtension {
 public:
  WebSocketExtension();
  bool Negotiate(const std::string& extensions, std::string* response);

 private:
  ::base::flat_map<std::string, std::unique_ptr<WebSocketExtensionInterface>>
      extensions_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_EXTENSION_H_