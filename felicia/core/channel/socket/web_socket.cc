#include "felicia/core/channel/socket/web_socket.h"

#include "third_party/chromium/base/base64.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/sha1.h"
#include "third_party/chromium/base/strings/string_tokenizer.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

static const size_t kHeaderSize = Bytes::FromKilloBytes(1).bytes();
::base::TimeDelta g_timeout = ::base::TimeDelta::FromSeconds(10);

constexpr const char* kConnection = "connection";
constexpr const char* kUpgrade = "upgrade";
constexpr const char* kHost = "host";
constexpr const char* kSecWebSocketKey = "sec-websocket-key";
constexpr const char* kSecWebSocketVersion = "sec-websocket-version";
constexpr const char* kSecWebSocketExtensions = "sec-websocket-extensions";
constexpr const char* kHandshakeGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WebSocket::HandshakeHandler::HandshakeHandler(WebSocket* websocket)
    : websocket_(websocket),
      buffer_(::base::MakeRefCounted<::net::GrowableIOBuffer>()) {}

WebSocket::HandshakeHandler::~HandshakeHandler() = default;

void WebSocket::HandshakeHandler::Handle(
    std::unique_ptr<::net::TCPSocket> socket) {
  DCHECK(!socket_);
  timeout_.Reset(::base::BindOnce(&WebSocket::HandshakeHandler::SendError,
                                  ::base::Unretained(this),
                                  ::net::HTTP_REQUEST_TIMEOUT));
  ::base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE, timeout_.callback(), g_timeout);

  socket_ = std::move(socket);
  if (buffer_->capacity() == 0) {
    buffer_->SetCapacity(kHeaderSize);
  }
  buffer_->set_offset(0);
  ReadHeader();
}

void WebSocket::HandshakeHandler::ReadHeader() {
  int to_read = buffer_->RemainingCapacity();
  if (to_read <= 0) {
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  int rv =
      socket_->Read(buffer_.get(), to_read,
                    ::base::BindOnce(&WebSocket::HandshakeHandler::OnReadHeader,
                                     ::base::Unretained(this)));
  if (rv == ::net::ERR_IO_PENDING) return;

  if (rv > 0) {
    buffer_->set_offset(buffer_->offset() + rv);
  }
  OnReadHeader(0);
}

void WebSocket::HandshakeHandler::OnReadHeader(int result) {
  if (result >= 0) {
    buffer_->set_offset(buffer_->offset() + result);
    Parse();
  } else {
    SendError(::net::HTTP_INTERNAL_SERVER_ERROR);
  }
}

void WebSocket::HandshakeHandler::Parse() {
  std::string header(buffer_->StartOfBuffer(), buffer_->offset());
  DLOG(INFO) << "Header received: " << header;

  size_t first_status_line =
      ::base::StringPiece(header.data(), header.length()).find_first_of("\r\n");
  if (first_status_line == ::base::StringPiece::npos) {
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  std::string::const_iterator begin = header.cbegin();
  std::string::const_iterator end = std::find(begin, header.cend(), ' ');

  if (end == header.end()) {
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  std::string method = std::string(begin, end);
  if (!::base::EqualsCaseInsensitiveASCII(method, "GET")) {
    DLOG(ERROR) << "method mismatched " << method;
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  // uri
  begin = end + 1;
  end = std::find(begin, header.cend(), ' ');

  if (end == header.end()) {
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  std::string version =
      std::string(end + 1, header.cbegin() + first_status_line);

  if (!::base::EqualsCaseInsensitiveASCII(version, "HTTP/1.1")) {
    DLOG(ERROR) << "version mismatched " << version;
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  ::base::StringTokenizer lines(header.cbegin() + first_status_line + 1,
                                header.cend(), "\r\n");
  headers_.clear();
  while (lines.GetNext()) {
    std::string::const_iterator line_begin = lines.token_begin();
    std::string::const_iterator line_end = lines.token_end();

    std::string::const_iterator end = std::find(line_begin, line_end, ':');
    if (line_end == end) continue;

    std::string key, value;
    key = ::base::ToLowerASCII(::base::TrimWhitespaceASCII(
        std::string(line_begin, end), ::base::TrimPositions::TRIM_ALL));
    ::base::TrimWhitespaceASCII(std::string(end + 1, line_end),
                                ::base::TrimPositions::TRIM_ALL, &value);

    headers_[key] = value;
  }

  Validate();
}

void WebSocket::HandshakeHandler::Validate() {
  const std::string& connection = headers_[kConnection];
  if (!::base::EqualsCaseInsensitiveASCII(connection, "upgrade")) {
    DLOG(ERROR) << "connection mismatched " << connection;
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  const std::string& upgrade = headers_[kUpgrade];
  if (!::base::EqualsCaseInsensitiveASCII(upgrade, "websocket")) {
    DLOG(ERROR) << "upgrade mismatched " << upgrade;
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  const std::string& version = headers_[kSecWebSocketVersion];
  if (!::base::EqualsCaseInsensitiveASCII(version, "13")) {
    DLOG(ERROR) << "sec-websocket-version mismatched " << version;
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  std::string key = headers_[kSecWebSocketKey];
  if (key.empty()) {
    SendError(::net::HTTP_BAD_REQUEST);
    return;
  }

  key.append(kHandshakeGuid);
  std::string sha1_hashed = ::base::SHA1HashString(key);
  std::string base64_encoded;
  ::base::Base64Encode(sha1_hashed, &base64_encoded);

  SendOK(base64_encoded);
}

void WebSocket::HandshakeHandler::SendOK(const std::string& key) {
  timeout_.Cancel();

  status_ = Status::OK();

  ::net::HttpStatusCode code = ::net::HTTP_SWITCHING_PROTOCOLS;

  const char* reason = ::net::GetHttpReasonPhrase(code);

  std::string response = ::base::StringPrintf(
      "HTTP/1.1 %d %s\r\n"
      "Connection: Upgrade\r\n"
      "Upgrade: websocket\r\n"
      "Sec-WebSocket-Accept: %s\r\n\r\n",
      static_cast<int>(code), reason, key.c_str());

  WriteResponse(response);
}

void WebSocket::HandshakeHandler::SendError(::net::HttpStatusCode code) {
  timeout_.Cancel();

  const char* reason = ::net::GetHttpReasonPhrase(code);

  if (code == ::net::HTTP_BAD_REQUEST) {
    status_ = errors::InvalidArgument(reason);
  } else if (code == ::net::HTTP_REQUEST_TIMEOUT) {
    status_ = errors::ResourceExhausted(reason);
  } else {
    status_ = errors::Unknown(reason);
  }

  std::string response = ::base::StringPrintf(
      "HTTP/1.1 %d %s\r\n"
      "Connection: close\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: %zd\r\n\r\n"
      "%s",
      static_cast<int>(code), reason, strlen(reason), reason);

  WriteResponse(response);
}

void WebSocket::HandshakeHandler::WriteResponse(const std::string& response) {
  int to_write = response.length();
  int written = 0;
  while (to_write > 0) {
    scoped_refptr<::net::IOBufferWithSize> buffer =
        ::base::MakeRefCounted<::net::IOBufferWithSize>(to_write);
    memcpy(buffer->data(), response.data() + written, to_write);
    int rv = socket_->Write(
        buffer.get(), buffer->size(),
        ::base::BindOnce(&WebSocket::HandshakeHandler::OnWriteResponse,
                         ::base::Unretained(this)),
        ::net::DefineNetworkTrafficAnnotation("web_socket_server",
                                              "Send Message"));

    if (rv == ::net::ERR_IO_PENDING) break;

    if (rv > 0) {
      to_write -= rv;
      written += rv;
    }

    if (to_write == 0 || rv <= 0) {
      OnWriteResponse(rv);
      break;
    }
  }
}

void WebSocket::HandshakeHandler::OnWriteResponse(int result) {
  if (status_.ok()) {
    websocket_->OnHandshaked(std::move(socket_));
  } else {
    socket_.reset();
    websocket_->OnHandshaked(status_);
  }
}

WebSocket::WebSocket() = default;
WebSocket::~WebSocket() = default;

bool WebSocket::IsWebSocket() const { return true; }

WebSocketServer* WebSocket::ToWebSocketServer() {
  DCHECK(IsServer());
  return reinterpret_cast<WebSocketServer*>(this);
}

}  // namespace felicia