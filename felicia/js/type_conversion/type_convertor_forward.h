// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_JS_TYPE_CONVERSION_TYPE_CONVERTOR_FORWARD_H_
#define FELICIA_JS_TYPE_CONVERSION_TYPE_CONVERTOR_FORWARD_H_

#if defined(FEL_NODE_BINDING)

#include "napi.h"

namespace felicia {
namespace js {

template <typename T, typename SFINAE = void>
class TypeConvertor;

}  // namespace js
}  // namespace felicia

#endif  // defined(FEL_NODE_BINDING)

#endif  // FELICIA_JS_TYPE_CONVERSION_TYPE_CONVERTOR_FORWARD_H_