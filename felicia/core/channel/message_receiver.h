#ifndef FELICIA_CORE_CHANNEL_MESSAGE_RECEIVER_H_
#define FELICIA_CORE_CHANNEL_MESSAGE_RECEIVER_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/header.h"
#include "felicia/core/message/message_io.h"

namespace felicia {

typedef base::RepeatingCallback<int()> HeaderSizeCallback;

typedef base::RepeatingCallback<MessageIOError(
    const char* buffer, int* message_offset, int* message_size)>
    ParseHeaderCallback;

template <typename T>
class MessageReceiver {
 public:
  explicit MessageReceiver(Channel* channel = nullptr) : channel_(channel) {}
  ~MessageReceiver() = default;

  void Reset() {
    channel_ = nullptr;
    receive_callback_.Reset();
    header_size_callback_.Reset();
    parse_header_callback_.Reset();
  }

  void set_channel(Channel* channel) { channel_ = channel; }

  // If you want to attach custom header, you need to add callback using this.
  void set_header_size_callback(HeaderSizeCallback header_size_callback) {
    header_size_callback_ = header_size_callback;
  }
  // If you want to attach custom header, you need to add callback using this.
  void set_parse_header_callback(ParseHeaderCallback parse_header_callback) {
    parse_header_callback_ = parse_header_callback;
  }

  T&& message() && { return std::move(message_); }
  T& message() & { return message_; }
  const T& message() const& { return message_; }

  void ReceiveMessage(StatusOnceCallback callback) {
    receive_callback_ = std::move(callback);
    if (channel_->ShouldReceiveMessageWithHeader()) {
      int unused = 0;
      channel_->ReceiveInternalBuffer(
          unused,
          base::BindOnce(&MessageReceiver<T>::OnReceiveMessageWithHeader,
                         base::Unretained(this)));
    } else {
      int header_size = this->header_size();
      if (header_size <= 0) {
        std::move(receive_callback_)
            .Run(errors::Aborted("header_size is not positive"));
        return;
      }
      channel_->ReceiveInternalBuffer(
          header_size, base::BindOnce(&MessageReceiver<T>::OnReceiveHeader,
                                      base::Unretained(this)));
    }
  }

  void OnReceiveMessageWithHeader(const Status& s) {
    if (!s.ok()) {
      std::move(receive_callback_).Run(s);
      return;
    }

    int message_offset;
    int message_size;
    const char* buffer = channel_->receive_buffer_.StartOfBuffer();
    MessageIOError err = ParseHeader(buffer, &message_offset, &message_size);
    if (err == MessageIOError::OK) {
      err = MessageIO<T>::Deserialize(buffer + message_offset, message_size,
                                      &message_);
    }

    if (err != MessageIOError::OK) {
      std::move(receive_callback_)
          .Run(errors::Aborted(MessageIOErrorToString(err)));
    } else {
      std::move(receive_callback_).Run(Status::OK());
    }
  }

  void OnReceiveHeader(const Status& s) {
    if (!s.ok()) {
      std::move(receive_callback_).Run(s);
      return;
    }

    int unused;
    int message_size;
    const char* buffer = channel_->receive_buffer_.StartOfBuffer();
    MessageIOError err = ParseHeader(buffer, &unused, &message_size);
    if (err != MessageIOError::OK) {
      std::move(receive_callback_)
          .Run(errors::Aborted(MessageIOErrorToString(err)));
      return;
    }

    if (message_size <= 0) {
      std::move(receive_callback_)
          .Run(errors::Aborted("message_size is not positive"));
      return;
    }

    channel_->ReceiveInternalBuffer(
        message_size, base::BindOnce(&MessageReceiver<T>::OnReceiveMessage,
                                     base::Unretained(this), message_size));
  }

  void OnReceiveMessage(int message_size, const Status& s) {
    const char* buffer = channel_->receive_buffer_.StartOfBuffer();
    MessageIOError err =
        MessageIO<T>::Deserialize(buffer, message_size, &message_);
    if (err != MessageIOError::OK) {
      std::move(receive_callback_)
          .Run(errors::Aborted(MessageIOErrorToString(err)));
    } else {
      std::move(receive_callback_).Run(Status::OK());
    }
  }

 private:
  int header_size() const {
    if (header_size_callback_.is_null()) {
      return header_.header_size();
    } else {
      return header_size_callback_.Run();
    }
  }

  MessageIOError ParseHeader(const char* buffer, int* message_offset,
                             int* message_size) {
    if (parse_header_callback_.is_null()) {
      return header_.ParseHeader(buffer, message_offset, message_size);
    } else {
      return parse_header_callback_.Run(buffer, message_offset, message_size);
    }
  }

  // not owned
  Channel* channel_;
  // Default header to parse serialized message.
  Header header_;
  T message_;
  StatusOnceCallback receive_callback_;
  HeaderSizeCallback header_size_callback_;
  ParseHeaderCallback parse_header_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_MESSAGE_RECEIVER_H_