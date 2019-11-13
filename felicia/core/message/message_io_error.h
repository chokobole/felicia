// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSAGE_MESSAGE_IO_ERROR_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_IO_ERROR_H_

#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

enum MessageIOError {
#define MESSAGE_IO_ERR(ERR, _) ERR,
#include "felicia/core/message/message_io_error_list.h"
#undef MESSAGE_IO_ERR
};

FEL_EXPORT std::string MessageIOErrorToString(MessageIOError mesasge_io_error);

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_IO_ERROR_H_