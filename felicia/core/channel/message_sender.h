#ifndef FELICIA_CORE_CHANNEL_MESSAGE_SENDER_H_
#define FELICIA_CORE_CHANNEL_MESSAGE_SENDER_H_

#include "third_party/chromium/base/callback.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/header.h"
#include "felicia/core/message/message_io.h"

namespace felicia {

typedef base::OnceCallback<MessageIOError(const std::string& content,
                                          std::string* text)>
    AttachHeaderCallback;

template <typename MessageTy>
class MessageSender {
 public:
  explicit MessageSender(Channel* channel = nullptr) : channel_(channel) {}
  ~MessageSender() = default;

  void set_channel(Channel* channel) { channel_ = channel; }

  // If you want to attach custom header, you need to add callback using this.
  void set_attach_header_callback(AttachHeaderCallback attach_header_callback) {
    attach_header_callback_ = std::move(attach_header_callback);
  }

  void SendMessage(const MessageTy& message, StatusOnceCallback callback) {
    std::string content;
    MessageIOError err = MessageIO<MessageTy>::Serialize(&message, &content);
    if (err == MessageIOError::OK) {
      std::string text;
      if (!channel_->HasNativeHeader()) {
        if (attach_header_callback_.is_null()) {
          Header header;
          int to_send = sizeof(Header) + content.length();
          if (channel_->send_buffer_.SetEnoughCapacityIfDynamic(to_send)) {
            err = header.AttachHeaderInternally(
                content, channel_->send_buffer_.StartOfBuffer());
            if (err == MessageIOError::OK) {
              channel_->SendInternalBuffer(to_send, std::move(callback));
              return;
            }
          } else {
            err = MessageIOError::ERR_NOT_ENOUGH_BUFFER;
          }
        } else {
          err = std::move(attach_header_callback_).Run(content, &text);
        }
      } else {
        text = std::move(content);
      }
      if (err == MessageIOError::OK) {
        channel_->Send(text, std::move(callback));
        return;
      }
    }

    std::move(callback).Run(errors::Aborted(MessageIOErrorToString(err)));
  }

 private:
  Channel* channel_;
  AttachHeaderCallback attach_header_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_MESSAGE_SENDER_H_