#ifndef FELICIA_CORE_CHANNEL_CHANNEL_BUFFER_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_BUFFER_H_

#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

class FEL_EXPORT ChannelBuffer {
 public:
  ChannelBuffer();
  ~ChannelBuffer();

  // GrowableIOBuffer methods
  void SetCapacity(Bytes bytes);
  void SetCapacity(int capacity);
  int capacity();

  void set_offset(int offset);
  int offset();

  int RemainingCapacity();
  char* StartOfBuffer();

  void SetDynamicBuffer(bool is_dynamic);
  void Reset();
  // Return true if capacity() is higher than or equal to |bytes|,
  // but if |is_dynamic_| is true, set capacity to |btyes| and
  // return true.
  bool SetEnoughCapacityIfDynamic(Bytes bytes);
  // Same with above.
  bool SetEnoughCapacityIfDynamic(int capacity);

  scoped_refptr<net::GrowableIOBuffer> buffer();

 private:
  scoped_refptr<net::GrowableIOBuffer> buffer_;
  bool is_dynamic_ = false;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SEND_BUFFER_H_