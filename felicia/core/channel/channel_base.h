#ifndef FELICIA_CORE_CHANNEL_CHANNEL_BASE_
#define FELICIA_CORE_CHANNEL_CHANNEL_BASE_

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/lib/error/status.h"

namespace felicia {

class ChannelBase {
 public:
  ChannelBase() = default;
  virtual ~ChannelBase() = default;

  virtual bool IsClient() const { return false; }
  virtual bool IsServer() const { return false; }

  virtual void Write(::net::IOBuffer* buf, size_t buf_len,
                     StatusCallback callback) = 0;
  virtual void Read(::net::IOBuffer* buf, size_t buf_len,
                    StatusCallback callback) = 0;

  static const int32_t kMaxReceiverBufferSize;

 protected:
  virtual void OnWrite(int result);
  virtual void OnRead(int result);

  StatusCallback write_callback_;
  StatusCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(ChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_BASE_