// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/base/mac/mach_port_broker.h

#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_MACH_PORT_BROKER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_MACH_PORT_BROKER_H_

#include <mach/mach.h>

#include <map>
#include <memory>
#include <string>

#include "third_party/chromium/base/mac/dispatch_source_mach.h"
#include "third_party/chromium/base/mac/scoped_mach_port.h"
#include "third_party/chromium/base/process/port_provider_mac.h"
#include "third_party/chromium/base/process/process_handle.h"
#include "third_party/chromium/base/synchronization/lock.h"

#include "felicia/core/lib/error/status.h"

namespace felicia {

class MachPortBroker : public ::base::PortProvider {
 public:
  static Status SendTaskPortToService(const std::string& service_name,
                                      mach_port_t port_to_send);

  MachPortBroker();
  ~MachPortBroker() override;

  const std::string& service_name() const { return service_name_; }

  // Performs any initialization work.
  Status Init();

  // Removes |pid| from the task port map. Callers MUST acquire the lock given
  // by GetLock() before calling this method (and release the lock afterwards).
  void InvalidatePid(base::ProcessHandle pid);

  // The lock that protects this MachPortBroker object. Callers MUST acquire
  // and release this lock around calls to AddPlaceholderForPid(),
  // InvalidatePid(), and FinalizePid();
  base::Lock& GetLock() { return lock_; }

  // Implement |base::PortProvider|.
  mach_port_t TaskForPid(::base::ProcessHandle process) const override;

 private:
  // Message handler that is invoked on |dispatch_source_| when an
  // incoming message needs to be received.
  void HandleRequest();

  bool AddTaskPort(base::ProcessHandle pid, mach_port_t task_port);

  std::string service_name_;

  // The Mach port on which the server listens.
  base::mac::ScopedMachReceiveRight server_port_;

  // The dispatch source and queue on which Mach messages will be received.
  std::unique_ptr<base::DispatchSourceMach> dispatch_source_;

  // Stores mach info for every process in the broker.
  typedef std::map<::base::ProcessHandle, mach_port_t> MachMap;
  MachMap mach_map_;

  // Mutex that guards |mach_map_|.
  mutable ::base::Lock lock_;

  DISALLOW_COPY_AND_ASSIGN(MachPortBroker);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_MACH_PORT_BROKER_H_