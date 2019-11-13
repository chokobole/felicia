// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_CONTAINERS_DATA_CONSTANTS_H_
#define FELICIA_CORE_LIB_CONTAINERS_DATA_CONSTANTS_H_

#include "felicia/core/lib/containers/data_internal.h"
#include "felicia/core/protobuf/data.pb.h"

namespace felicia {

#define FEL_MAKE_DATA_TYPE(ELEMENT_NAME, ELEMENT_TYPE, CHANNEL_NAME, \
                           CHANNEL_TYPE)                             \
  constexpr uint32_t DATA_TYPE_##ELEMENT_NAME##_##CHANNEL_NAME =     \
      ::felicia::internal::MakeDataMessageType(ELEMENT_TYPE, CHANNEL_TYPE)

#define FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(ELEMENT_NAME, ELEMENT_TYPE) \
  FEL_MAKE_DATA_TYPE(ELEMENT_NAME, ELEMENT_TYPE, C1,                     \
                     ::felicia::DataMessage::CHANNEL_TYPE_C1);           \
  FEL_MAKE_DATA_TYPE(ELEMENT_NAME, ELEMENT_TYPE, C2,                     \
                     ::felicia::DataMessage::CHANNEL_TYPE_C2);           \
  FEL_MAKE_DATA_TYPE(ELEMENT_NAME, ELEMENT_TYPE, C3,                     \
                     ::felicia::DataMessage::CHANNEL_TYPE_C3);           \
  FEL_MAKE_DATA_TYPE(ELEMENT_NAME, ELEMENT_TYPE, C4,                     \
                     ::felicia::DataMessage::CHANNEL_TYPE_C4)

FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(8U,
                                     ::felicia::DataMessage::ELEMENT_TYPE_8U);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(8S,
                                     ::felicia::DataMessage::ELEMENT_TYPE_8S);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(16U,
                                     ::felicia::DataMessage::ELEMENT_TYPE_16U);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(16S,
                                     ::felicia::DataMessage::ELEMENT_TYPE_16S);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(32U,
                                     ::felicia::DataMessage::ELEMENT_TYPE_32U);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(32S,
                                     ::felicia::DataMessage::ELEMENT_TYPE_32S);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(64U,
                                     ::felicia::DataMessage::ELEMENT_TYPE_64U);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(64S,
                                     ::felicia::DataMessage::ELEMENT_TYPE_64S);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(32F,
                                     ::felicia::DataMessage::ELEMENT_TYPE_32F);
FEL_MAKE_DATA_TYPE_FOR_EACH_CHANNELS(64F,
                                     ::felicia::DataMessage::ELEMENT_TYPE_64F);
constexpr uint32_t DATA_TYPE_CUSTOM_C1 = internal::MakeDataMessageType(
    DataMessage::ELEMENT_TYPE_CUSTOM, DataMessage::CHANNEL_TYPE_C1);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_CONTAINERS_DATA_CONSTANTS_H_