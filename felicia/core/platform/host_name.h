/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
// Modifications copyright (C) 2019 felicia

#ifndef FELICIA_CORE_PLATFORM_HOST_NAME_H_
#define FELICIA_CORE_PLATFORM_HOST_NAME_H_

#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace net {

// Return the hostname of the machine on which this process is running
EXPORT std::string Hostname();

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_HOST_NAME_H_
