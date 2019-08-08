#include "felicia/core/channel/channel_buffer.h"

namespace felicia {

void ChannelBuffer::EnsureBuffer() {
  if (!buffer_) buffer_ = base::MakeRefCounted<net::GrowableIOBuffer>();
}

void ChannelBuffer::SetCapacity(Bytes bytes) { SetCapacity(bytes.bytes()); }

void ChannelBuffer::SetCapacity(int capacity) {
  EnsureBuffer();
  buffer_->SetCapacity(capacity);
}

int ChannelBuffer::capacity() {
  EnsureBuffer();
  return buffer_->capacity();
}

void ChannelBuffer::set_offset(int offset) {
  EnsureBuffer();
  buffer_->set_offset(offset);
}

int ChannelBuffer::offset() {
  EnsureBuffer();
  return buffer_->offset();
}

int ChannelBuffer::RemainingCapacity() {
  EnsureBuffer();
  return buffer_->RemainingCapacity();
}

char* ChannelBuffer::StartOfBuffer() {
  EnsureBuffer();
  return buffer_->StartOfBuffer();
}

scoped_refptr<net::GrowableIOBuffer> ChannelBuffer::buffer() { return buffer_; }

void ChannelBuffer::EnableDynamicBuffer() { is_dynamic_ = true; }

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

void SendBuffer::InvalidateAttachment() { attach_kind_ = ATTACH_KIND_NONE; }

void SendBuffer::AttachGeneral(int size) {
  attach_kind_ = ATTACH_KIND_GENERAL;
  size_ = size;
}

void SendBuffer::AttachWebSocket(int size) {
  attach_kind_ = ATTACH_KIND_WEB_SOCKET;
  size_ = size;
}

bool SendBuffer::CanReuse(AttachKind attach_kind) const {
  return attach_kind_ == attach_kind;
}

int SendBuffer::size() const { return size_; }

}  // namespace felicia