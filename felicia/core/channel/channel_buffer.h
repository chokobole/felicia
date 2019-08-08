#ifndef FELICIA_CORE_CHANNEL_CHANNEL_BUFFER_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_BUFFER_H_

#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

class ChannelBuffer {
 public:
  // GrowableIOBuffer methods
  void SetCapacity(Bytes bytes);
  void SetCapacity(int capacity);
  int capacity();

  void set_offset(int offset);
  int offset();

  int RemainingCapacity();
  char* StartOfBuffer();

  void EnableDynamicBuffer();
  void Reset();
  // Return true if capacity() is higher than or equal to |bytes|,
  // but if |is_dynamic_| is true, set capacity to |btyes| and
  // return true.
  bool SetEnoughCapacityIfDynamic(Bytes bytes);
  // Same with above.
  bool SetEnoughCapacityIfDynamic(int capacity);

  scoped_refptr<net::GrowableIOBuffer> buffer();

 private:
  void EnsureBuffer();

  scoped_refptr<net::GrowableIOBuffer> buffer_;
  bool is_dynamic_ = false;
};

class SendBuffer : public ChannelBuffer {
 public:
  enum AttachKind {
    ATTACH_KIND_NONE = 0,
    ATTACH_KIND_GENERAL = 1,
    ATTACH_KIND_WEB_SOCKET = 2,
  };

  void InvalidateAttachment();

  void AttachGeneral(int size);
  void AttachWebSocket(int size);

  bool CanReuse(AttachKind attach_kind) const;

  int size() const;

 private:
  int attach_kind_ = ATTACH_KIND_NONE;
  int size_ = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SEND_BUFFER_H_