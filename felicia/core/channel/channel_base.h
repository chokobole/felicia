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

  virtual void Write(char* buffer, int size, StatusOnceCallback callback) = 0;
  virtual void Read(char* buffer, int size, StatusOnceCallback callback) = 0;

 protected:
  static void CallbackWithStatus(StatusOnceCallback callback, int result);

  StatusOnceCallback write_callback_;
  StatusOnceCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(ChannelBase);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_BASE_