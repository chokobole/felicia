// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/base/mac/mach_port_broker.mm

#include "felicia/core/channel/shared_memory/mach_port_broker.h"

#include <bsm/libbsm.h>
#include <servers/bootstrap.h>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/mac/foundation_util.h"
#include "third_party/chromium/base/mac/mach_logging.h"
#include "third_party/chromium/base/mac/scoped_mach_msg_destroy.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

namespace {

constexpr mach_msg_id_t kTaskPortMessageId = 'tskp';

// Mach message structure used in the client as a sending message.
struct MachSendPortMessage {
  mach_msg_header_t header;
  mach_msg_body_t body;
  mach_msg_port_descriptor_t task_port;
};

// Complement to the MachSendPortMessage, this is used in the server for receiving
// a message. Contains a message trailer with audit information.
struct MachReceivePortMessage : public MachSendPortMessage {
  mach_msg_audit_trailer_t trailer;
};

}  // namespace

// static
Status MachPortBroker::SendTaskPortToService(const std::string& service_name,
                                             mach_port_t port_to_send) {
  // Look up the named MachPortBroker port that's been registered with the
  // bootstrap server.
  base::mac::ScopedMachSendRight port;
  kern_return_t kr = bootstrap_look_up(bootstrap_port, service_name.c_str(),
                                       base::mac::ScopedMachSendRight::Receiver(port).get());
  if (kr != KERN_SUCCESS) {
    BOOTSTRAP_LOG(ERROR, kr) << "bootstrap_look_up " << service_name;
    return errors::NotFound(
        base::StringPrintf("Failed to boostrap_look_up: %s.", service_name.c_str()));
  }

  // Create the check in message. This will copy a send right on this process'
  // task port and send it to the server.
  MachSendPortMessage msg{};
  msg.header.msgh_bits = MACH_MSGH_BITS_REMOTE(MACH_MSG_TYPE_COPY_SEND) | MACH_MSGH_BITS_COMPLEX;
  msg.header.msgh_size = sizeof(msg);
  msg.header.msgh_remote_port = port.get();
  msg.header.msgh_id = kTaskPortMessageId;
  msg.body.msgh_descriptor_count = 1;
  msg.task_port.name = port_to_send;
  msg.task_port.disposition = MACH_MSG_TYPE_PORT_SEND;
  msg.task_port.type = MACH_MSG_PORT_DESCRIPTOR;

  kr = mach_msg(&msg.header, MACH_SEND_MSG | MACH_SEND_TIMEOUT, sizeof(msg), 0, MACH_PORT_NULL,
                100 /*milliseconds*/, MACH_PORT_NULL);
  if (kr != KERN_SUCCESS) {
    MACH_LOG(ERROR, kr) << "mach_msg";
    return errors::Unavailable("Failed to send mach message.");
  }

  return Status::OK();
}

mach_port_t MachPortBroker::TaskForPid(base::ProcessHandle pid) const {
  base::AutoLock lock(lock_);
  MachPortBroker::MachMap::const_iterator it = mach_map_.find(pid);
  if (it == mach_map_.end()) return MACH_PORT_NULL;
  return it->second;
}

MachPortBroker::MachPortBroker() = default;

MachPortBroker::~MachPortBroker() = default;

Status MachPortBroker::Init() {
  DCHECK(server_port_.get() == MACH_PORT_NULL);

  // Check in with launchd and publish the service name.
  std::string service_name = base::StringPrintf("%s.felicia.%d.%I64u", base::mac::BaseBundleID(),
                                                getpid(), base::RandUint64());
  kern_return_t kr =
      bootstrap_check_in(bootstrap_port, service_name.c_str(),
                         base::mac::ScopedMachReceiveRight::Receiver(server_port_).get());
  if (kr != KERN_SUCCESS) {
    BOOTSTRAP_LOG(ERROR, kr) << "bootstrap_check_in " << service_name;
    return errors::Unavailable(
        base::StringPrintf("Failed to bootstrap_check_in: %s.", service_name.c_str()));
  }

  service_name_ = service_name;

  // Start the dispatch source.
  std::string queue_name = base::StringPrintf("%s.MachPortBroker", base::mac::BaseBundleID());
  dispatch_source_.reset(new base::DispatchSourceMach(queue_name.c_str(), server_port_.get(), ^{
    HandleRequest();
  }));
  dispatch_source_->Resume();

  return Status::OK();
}

void MachPortBroker::InvalidatePid(base::ProcessHandle pid) {
  lock_.AssertAcquired();

  MachMap::iterator mach_it = mach_map_.find(pid);
  if (mach_it != mach_map_.end()) {
    kern_return_t kr = mach_port_deallocate(mach_task_self(), mach_it->second);
    MACH_LOG_IF(WARNING, kr != KERN_SUCCESS, kr) << "mach_port_deallocate";
    mach_map_.erase(mach_it);
  }
}

void MachPortBroker::HandleRequest() {
  MachReceivePortMessage msg{};
  msg.header.msgh_size = sizeof(msg);
  msg.header.msgh_local_port = server_port_.get();

  const mach_msg_option_t options = MACH_RCV_MSG | MACH_RCV_TRAILER_TYPE(MACH_RCV_TRAILER_AUDIT) |
                                    MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_AUDIT);

  kern_return_t kr = mach_msg(&msg.header, options, 0, sizeof(msg), server_port_.get(),
                              MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  if (kr != KERN_SUCCESS) {
    MACH_LOG(ERROR, kr) << "mach_msg";
    return;
  }

  // Destroy any rights that this class does not take ownership of.
  base::ScopedMachMsgDestroy scoped_msg(&msg.header);

  // Validate that the received message is what is expected.
  if ((msg.header.msgh_bits & MACH_MSGH_BITS_COMPLEX) == 0 ||
      msg.header.msgh_id != kTaskPortMessageId ||
      msg.header.msgh_size != sizeof(MachSendPortMessage) ||
      msg.task_port.disposition != MACH_MSG_TYPE_PORT_SEND ||
      msg.task_port.type != MACH_MSG_PORT_DESCRIPTOR) {
    LOG(ERROR) << "Received unexpected message";
    return;
  }

  // Use the kernel audit information to make sure this message is from
  // a task that this process spawned. The kernel audit token contains the
  // unspoofable pid of the task that sent the message.
  pid_t pid = audit_token_to_pid(msg.trailer.msgh_audit);
  mach_port_t task_port = msg.task_port.name;

  // Take the lock and update the broker information.
  {
    base::AutoLock lock(lock_);
    if (AddTaskPort(pid, task_port)) {
      scoped_msg.Disarm();
    }
  }
  NotifyObservers(pid);
}

bool MachPortBroker::AddTaskPort(base::ProcessHandle pid, mach_port_t task_port) {
  lock_.AssertAcquired();

  MachMap::iterator it = mach_map_.find(pid);
  if (it == mach_map_.end()) {
    mach_map_[pid] = task_port;
    return true;
  }

  NOTREACHED();
  return false;
}

}  // namespace felicia