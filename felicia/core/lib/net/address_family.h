// Copyright 2010 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#ifndef FELICIA_CORE_LIB_NET_ADDRESS_FAMILY_
#define FELICIA_CORE_LIB_NET_ADDRESS_FAMILY_

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <sys/socket.h>
#elif defined(PLATFORM_WINDOWS)
#include <ws2tcpip.h>
#endif
#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace net {

class IPAddress;

enum AddressFamily {
  ADDRESS_FAMILY_UNSPECIFIED,  // AF_UNSPEC
  ADDRESS_FAMILY_IPV4,         // AF_INET
  ADDRESS_FAMILY_IPV6,         // AF_INET6
};

// Returns AddressFamily for |address|.
EXPORT AddressFamily GetAddressFamily(const IPAddress& address);

// Maps the given AddressFamily to either AF_INET, AF_INET6 or AF_UNSPEC.
EXPORT int ConvertAddressFamily(AddressFamily address_family);

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_NET_ADDRESS_FAMILY_