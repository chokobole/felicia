// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_BUFFER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_BUFFER_H_

#include "third_party/chromium/device/base/synchronization/shared_memory_seqlock_buffer.h"

namespace felicia {

class ReadOnlySharedBuffer;
class WritableSharedBuffer;

class SharedBuffer {
 public:
  typedef device::SharedMemorySeqLockBuffer<char*> SerializedBuffer;

  virtual ~SharedBuffer() = default;

  virtual bool IsReadOnlySharedBuffer() const { return false; }
  virtual bool IsWritableSharedBuffer() const { return false; }

  ReadOnlySharedBuffer* ToReadOnlySharedBuffer() {
    DCHECK(IsReadOnlySharedBuffer());
    return reinterpret_cast<ReadOnlySharedBuffer*>(this);
  }
  WritableSharedBuffer* ToWritableSharedBuffer() {
    DCHECK(IsWritableSharedBuffer());
    return reinterpret_cast<WritableSharedBuffer*>(this);
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_SHARED_BUFFER_H_