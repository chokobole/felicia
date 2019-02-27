// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/network_change_notifier.h"

#include "base/strings/string_util.h"
#include "net/base/network_interfaces.h"

namespace net {

// static
NetworkChangeNotifier::ConnectionType
NetworkChangeNotifier::ConnectionTypeFromInterfaceList(
    const NetworkInterfaceList& interfaces) {
  bool first = true;
  ConnectionType result = CONNECTION_NONE;
  for (size_t i = 0; i < interfaces.size(); ++i) {
#if defined(OS_WIN)
    if (interfaces[i].friendly_name == "Teredo Tunneling Pseudo-Interface")
      continue;
#endif
#if defined(OS_MACOSX)
    // Ignore tunnel and airdrop interfaces.
    if (base::StartsWith(interfaces[i].friendly_name, "utun",
                         base::CompareCase::SENSITIVE) ||
        base::StartsWith(interfaces[i].friendly_name, "awdl",
                         base::CompareCase::SENSITIVE)) {
      continue;
    }
#endif

    // Remove VMware network interfaces as they're internal and should not be
    // used to determine the network connection type.
    if (base::ToLowerASCII(interfaces[i].friendly_name).find("vmnet") !=
        std::string::npos) {
      continue;
    }
    if (first) {
      first = false;
      result = interfaces[i].type;
    } else if (result != interfaces[i].type) {
      return CONNECTION_UNKNOWN;
    }
  }
  return result;
}

}  // namespace net