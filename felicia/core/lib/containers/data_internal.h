// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_CONTAINERS_DATA_INTERNAL_H_
#define FELICIA_CORE_LIB_CONTAINERS_DATA_INTERNAL_H_

#include "felicia/core/protobuf/data.pb.h"

namespace felicia {
namespace internal {

constexpr void GetElementaAndChannelType(
    uint32_t type, DataMessage::ElementType* element_type,
    DataMessage::ChannelType* channel_type) {
  *element_type =
      static_cast<DataMessage::ElementType>(static_cast<uint16_t>(type >> 16));
  *channel_type =
      static_cast<DataMessage::ChannelType>(static_cast<uint16_t>(type));
}

constexpr uint32_t MakeDataMessageType(DataMessage::ElementType element_type,
                                       DataMessage::ChannelType channel_type) {
  return (static_cast<uint32_t>(element_type) << 16) |
         static_cast<uint32_t>(channel_type);
}

constexpr size_t GetElement1Size(DataMessage::ElementType element_type) {
  switch (element_type) {
    case DataMessage::ELEMENT_TYPE_8U:
    case DataMessage::ELEMENT_TYPE_8S: {
      return 1;
    }
    case DataMessage::ELEMENT_TYPE_16U:
    case DataMessage::ELEMENT_TYPE_16S: {
      return 2;
    }
    case DataMessage::ELEMENT_TYPE_32U:
    case DataMessage::ELEMENT_TYPE_32S:
    case DataMessage::ELEMENT_TYPE_32F: {
      return 4;
    }
    case DataMessage::ELEMENT_TYPE_64U:
    case DataMessage::ELEMENT_TYPE_64S:
    case DataMessage::ELEMENT_TYPE_64F: {
      return 8;
    }
    default:
      return -1;
  }
}

constexpr size_t GetChannelSize(DataMessage::ChannelType channel_type) {
  switch (channel_type) {
    case DataMessage::CHANNEL_TYPE_C1:
      return 1;
    case DataMessage::CHANNEL_TYPE_C2:
      return 2;
    case DataMessage::CHANNEL_TYPE_C3:
      return 3;
    case DataMessage::CHANNEL_TYPE_C4:
      return 4;
    default:
      return -1;
  }
}

}  // namespace internal
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_DATA_INTERNAL_H_