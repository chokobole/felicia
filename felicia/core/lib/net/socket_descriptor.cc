// Copyright 2013 The Chromium Authors. All rights reserved.
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

#include "felicia/core/lib/net/socket_descriptor.h"

#if defined(PLATFORM_WINDOWS)
#include <ws2tcpip.h>
#include "net/base/winsock_init.h"
#elif defined(PLATFORM_POSIX)
#include <sys/socket.h>
#include <sys/types.h>
#endif

#if defined(PLATFORM_MACOSX)
#include <unistd.h>
#endif

namespace felicia {
namespace net {

SocketDescriptor CreatePlatformSocket(int family, int type, int protocol) {
#if defined(PLATFORM_WINDOWS)
  EnsureWinsockInit();
  SocketDescriptor result =
      ::WSASocket(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
  if (result != kInvalidSocket && family == AF_INET6) {
    DWORD value = 0;
    if (setsockopt(result, IPPROTO_IPV6, IPV6_V6ONLY,
                   reinterpret_cast<const char*>(&value), sizeof(value))) {
      closesocket(result);
      return kInvalidSocket;
    }
  }
  return result;
#elif defined(PLATFORM_POSIX)
  SocketDescriptor result = ::socket(family, type, protocol);
#if defined(PLATFORM_MACOSX)
  // Disable SIGPIPE on this socket. Although Chromium globally disables
  // SIGPIPE, the net stack may be used in other consumers which do not do
  // this. SO_NOSIGPIPE is a Mac-only API. On Linux, it is a flag on send.
  if (result != kInvalidSocket) {
    int value = 1;
    if (setsockopt(result, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value))) {
      close(result);
      return kInvalidSocket;
    }
  }
#endif
  return result;
#endif  // PLATFORM_WINDOWS
}

}  // namespace net
}  // namespace felicia
