#ifndef FELICIA_CORE_CHANNEL_CHANNEL_BASE_
#define FELICIA_CORE_CHANNEL_CHANNEL_BASE_

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class TCPChannelBase;
class UDPChannelBase;

class EXPORT ChannelBase {
 public:
  ChannelBase();
  virtual ~ChannelBase();

  virtual bool IsClient() const;
  virtual bool IsServer() const;

  virtual bool IsTCPChannelBase() const;
  virtual bool IsUDPChannelBase() const;

  TCPChannelBase* ToTCPChannelBase();
  UDPChannelBase* ToUDPChannelBase();

  virtual void Write(::net::IOBuffer* buffer, int size,
                     StatusCallback callback) = 0;
  virtual void Read(::net::IOBuffer* buffer, int size,
                    StatusCallback callback) = 0;

  static size_t GetMaximumBufferSize();

 protected:
  static void CallbackWithStatus(StatusCallback callback, int result);

  StatusCallback write_callback_;
  StatusCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(ChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_BASE_