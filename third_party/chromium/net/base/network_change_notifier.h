// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_NETWORK_CHANGE_NOTIFIER_H_
#define NET_BASE_NETWORK_CHANGE_NOTIFIER_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "base/macros.h"
#include "net/base/net_export.h"

namespace net {

struct NetworkInterface;
typedef std::vector<NetworkInterface> NetworkInterfaceList;

// NetworkChangeNotifier monitors the system for network changes, and notifies
// registered observers of those events.  Observers may register on any thread,
// and will be called back on the thread from which they registered.
// NetworkChangeNotifiers are threadsafe, though they must be created and
// destroyed on the same thread.
class NET_EXPORT NetworkChangeNotifier {
 public:
  // This is a superset of the connection types in the NetInfo v3 specification:
  // http://w3c.github.io/netinfo/.
  //
  // A Java counterpart will be generated for this enum.
  // GENERATED_JAVA_ENUM_PACKAGE: org.chromium.net
  //
  // New enum values should only be added to the end of the enum and no values
  // should be modified or reused, as this is reported via UMA.
  enum ConnectionType {
    CONNECTION_UNKNOWN = 0,  // A connection exists, but its type is unknown.
                             // Also used as a default value.
    CONNECTION_ETHERNET = 1,
    CONNECTION_WIFI = 2,
    CONNECTION_2G = 3,
    CONNECTION_3G = 4,
    CONNECTION_4G = 5,
    CONNECTION_NONE = 6,     // No connection.
    CONNECTION_BLUETOOTH = 7,
    CONNECTION_LAST = CONNECTION_BLUETOOTH
  };

  // Gets the current connection type based on |interfaces|. Returns
  // CONNECTION_NONE if there are no interfaces, CONNECTION_UNKNOWN if two
  // interfaces have different connection types or the connection type of all
  // interfaces if they have the same interface type.
  static ConnectionType ConnectionTypeFromInterfaceList(
      const NetworkInterfaceList& interfaces);
};

}  // namespace net

#endif  // NET_BASE_NETWORK_CHANGE_NOTIFIER_H_
