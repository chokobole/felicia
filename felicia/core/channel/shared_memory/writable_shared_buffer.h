// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/device/gamepad/gamepad_shared_buffer.h

#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_WRITABLE_SHARED_BUFFER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_WRITABLE_SHARED_BUFFER_H_

#include "third_party/chromium/base/memory/writable_shared_memory_region.h"

#include "felicia/core/channel/shared_memory/shared_buffer.h"

namespace felicia {

class WritableSharedBuffer : public SharedBuffer {
 public:
  explicit WritableSharedBuffer(size_t size);
  ~WritableSharedBuffer();

  bool IsWritableSharedBuffer() const override;

  char* data();
  size_t size() const;

  ::base::ReadOnlySharedMemoryRegion DuplicateSharedMemoryRegion() const;

  void WriteBegin();
  void WriteEnd();

 private:
  ::base::ReadOnlySharedMemoryRegion shared_memory_region_;
  ::base::WritableSharedMemoryMapping shared_memory_mapping_;

  SerializedBuffer* buffer_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_WRITABLE_SHARED_BUFFER_H_