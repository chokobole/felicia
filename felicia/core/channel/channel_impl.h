#ifndef FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class Socket;

class ChannelImpl {
 public:
  virtual ~ChannelImpl();

  virtual bool IsSocket() const;

  Socket* ToSocket();

  virtual void Write(char* buffer, int size, StatusOnceCallback callback) = 0;
  virtual void Read(char* buffer, int size, StatusOnceCallback callback) = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_