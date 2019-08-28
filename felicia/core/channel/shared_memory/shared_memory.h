#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_MEMORY_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_MEMORY_H_

#include "third_party/chromium/base/memory/platform_shared_memory_region.h"

#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/channel/shared_memory/shared_buffer.h"

namespace felicia {

class SharedMemory : public ChannelImpl {
 public:
  explicit SharedMemory(size_t size);
  explicit SharedMemory(base::subtle::PlatformSharedMemoryRegion handle);
  ~SharedMemory();

  bool IsSharedMemory() const override { return true; }

  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

  size_t BufferSize() const;

  ChannelDef ToChannelDef() const;
  static std::unique_ptr<SharedMemory> FromChannelDef(ChannelDef channel_def);

 private:
  std::unique_ptr<SharedBuffer> buffer_;
  base::subtle::Atomic32 last_version_;  // Used when read the data
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_MEMORY_H_