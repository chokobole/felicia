#include "felicia/core/channel/channel_buffer.h"

namespace felicia {

ChannelBuffer::ChannelBuffer()
    : buffer_(base::MakeRefCounted<net::GrowableIOBuffer>()) {}

ChannelBuffer::~ChannelBuffer() = default;

void ChannelBuffer::SetCapacity(Bytes bytes) { SetCapacity(bytes.bytes()); }

void ChannelBuffer::SetCapacity(int capacity) {
  buffer_->SetCapacity(capacity);
}

int ChannelBuffer::capacity() { return buffer_->capacity(); }

void ChannelBuffer::set_offset(int offset) { buffer_->set_offset(offset); }

int ChannelBuffer::offset() { return buffer_->offset(); }

int ChannelBuffer::RemainingCapacity() { return buffer_->RemainingCapacity(); }

char* ChannelBuffer::StartOfBuffer() { return buffer_->StartOfBuffer(); }

scoped_refptr<net::GrowableIOBuffer> ChannelBuffer::buffer() { return buffer_; }

void ChannelBuffer::SetDynamicBuffer(bool is_dynamic) { is_dynamic_ = true; }

void ChannelBuffer::Reset() {
  if (!is_dynamic_ && capacity() == 0) {
    DLOG(WARNING) << "Buffer was not allocated, used default size.";
    SetCapacity(Bytes::FromKilloBytes(1));
  }
  set_offset(0);
}

bool ChannelBuffer::SetEnoughCapacityIfDynamic(Bytes bytes) {
  return SetEnoughCapacityIfDynamic(bytes.bytes());
}

bool ChannelBuffer::SetEnoughCapacityIfDynamic(int capacity) {
  if (this->capacity() >= capacity) return true;
  if (!is_dynamic_) return false;

  DLOG(INFO) << "Dynamically allocate buffer " << capacity;
  SetCapacity(capacity);
  return true;
}

}  // namespace felicia