#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_READONLY_SHARED_BUFFER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_READONLY_SHARED_BUFFER_H_

#include "third_party/chromium/base/memory/read_only_shared_memory_region.h"

#include "felicia/core/channel/shared_memory/shared_buffer.h"

namespace felicia {

class ReadOnlySharedBuffer : public SharedBuffer {
 public:
  explicit ReadOnlySharedBuffer(
      ::base::subtle::PlatformSharedMemoryRegion handle);
  ~ReadOnlySharedBuffer();

  bool IsReadOnlySharedBuffer() const override;

  const char* data() const;
  size_t size() const;

  ::base::subtle::Atomic32 ReadBegin() const;
  bool ReadRetry(::base::subtle::Atomic32 version) const;

 private:
  ::base::ReadOnlySharedMemoryRegion shared_memory_region_;
  ::base::ReadOnlySharedMemoryMapping shared_memory_mapping_;

  const SerializedBuffer* buffer_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_READONLY_SHARED_BUFFER_H_