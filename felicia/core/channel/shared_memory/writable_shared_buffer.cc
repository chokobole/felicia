// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/device/gamepad/gamepad_shared_buffer.cc

#include "felicia/core/channel/shared_memory/writable_shared_buffer.h"

namespace felicia {

WritableSharedBuffer::WritableSharedBuffer(size_t size) {
  ::base::MappedReadOnlyRegion mapped_region =
      ::base::ReadOnlySharedMemoryRegion::Create(size);
  CHECK(mapped_region.IsValid());
  shared_memory_region_ = std::move(mapped_region.region);
  shared_memory_mapping_ = std::move(mapped_region.mapping);

  char* mem = reinterpret_cast<char*>(shared_memory_mapping_.memory());
  DCHECK(mem);
  buffer_ = new (mem) SerializedBuffer();
}

WritableSharedBuffer::~WritableSharedBuffer() = default;

bool WritableSharedBuffer::IsWritableSharedBuffer() const { return true; }

char* WritableSharedBuffer::data() {
  return reinterpret_cast<char*>(buffer_) + sizeof(SerializedBuffer);
}

size_t WritableSharedBuffer::size() const {
  return shared_memory_region_.GetSize() - sizeof(SerializedBuffer);
}

::base::ReadOnlySharedMemoryRegion
WritableSharedBuffer::DuplicateSharedMemoryRegion() const {
  return shared_memory_region_.Duplicate();
}

void WritableSharedBuffer::WriteBegin() { buffer_->seqlock.WriteBegin(); }

void WritableSharedBuffer::WriteEnd() { buffer_->seqlock.WriteEnd(); }

}  // namespace felicia