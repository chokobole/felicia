#include "felicia/core/channel/shared_memory/read_only_shared_buffer.h"

namespace felicia {

ReadOnlySharedBuffer::ReadOnlySharedBuffer(
    ::base::subtle::PlatformSharedMemoryRegion handle) {
  shared_memory_region_ =
      ::base::ReadOnlySharedMemoryRegion::Deserialize(std::move(handle));
  shared_memory_mapping_ = std::move(shared_memory_region_.Map());
  buffer_ = reinterpret_cast<const SerializedBuffer*>(
      shared_memory_mapping_.memory());
}

ReadOnlySharedBuffer::~ReadOnlySharedBuffer() = default;

bool ReadOnlySharedBuffer::IsReadOnlySharedBuffer() const { return true; }

const char* ReadOnlySharedBuffer::data() const {
  return reinterpret_cast<const char*>(buffer_) + sizeof(SerializedBuffer);
}

size_t ReadOnlySharedBuffer::size() const {
  return shared_memory_region_.GetSize() - sizeof(SerializedBuffer);
}

::base::subtle::Atomic32 ReadOnlySharedBuffer::ReadBegin() const {
  return buffer_->seqlock.ReadBegin();
}

bool ReadOnlySharedBuffer::ReadRetry(::base::subtle::Atomic32 version) const {
  return buffer_->seqlock.ReadRetry(version);
}

}  // namespace felicia