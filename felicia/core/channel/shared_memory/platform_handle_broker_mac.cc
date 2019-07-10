#include "felicia/core/channel/shared_memory/platform_handle_broker.h"

#include "third_party/chromium/base/mac/mach_logging.h"
#include "third_party/chromium/base/mac/mach_port_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

namespace {

struct MachSendShmDataMessage {
  mach_msg_header_t header;
  mach_msg_body_t body;
  mach_msg_port_descriptor_t task_port;
  mach_msg_ool_descriptor_t data;
};

struct MachReceiveShmDataMessage : public MachSendShmDataMessage {
  mach_msg_audit_trailer_t trailer;
};

}  // namespace

mach_port_t MakeReceivingPort() {
  mach_port_t client_port;
  kern_return_t kr =
      mach_port_allocate(mach_task_self(),         // our task is acquiring
                         MACH_PORT_RIGHT_RECEIVE,  // a new receive right
                         &client_port);            // with this name
  MACH_CHECK(kr == KERN_SUCCESS, kr) << "mach_port_allocate";

  kr = mach_port_insert_right(mach_task_self(), client_port, client_port,
                              MACH_MSG_TYPE_MAKE_SEND);
  MACH_CHECK(kr == KERN_SUCCESS, kr) << "mach_port_insert_right";

  return client_port;
}

kern_return_t SendMachShmData(mach_port_t receiving_port,
                              mach_port_t port_to_send, const std::string& data,
                              int disposition) {
  MachSendShmDataMessage send_msg;
  mach_msg_header_t* send_hdr = &(send_msg.header);
  send_msg.header.msgh_bits =
      MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0) | MACH_MSGH_BITS_COMPLEX;
  send_msg.header.msgh_size = sizeof(send_msg);
  send_msg.header.msgh_remote_port = receiving_port;
  send_msg.header.msgh_local_port = MACH_PORT_NULL;
  send_msg.header.msgh_reserved = 0;
  send_msg.header.msgh_id = 0;
  send_msg.body.msgh_descriptor_count = 2;
  send_msg.task_port.name = port_to_send;
  send_msg.task_port.disposition = disposition;
  send_msg.task_port.type = MACH_MSG_PORT_DESCRIPTOR;
  send_msg.data.address = const_cast<char*>(data.c_str());
  send_msg.data.size = data.length();
  send_msg.data.deallocate = FALSE;
  send_msg.data.copy = MACH_MSG_PHYSICAL_COPY;
  send_msg.data.type = MACH_MSG_OOL_DESCRIPTOR;

  kern_return_t kr =
      mach_msg(send_hdr,               // message buffer
               MACH_SEND_MSG,          // option indicating send
               send_hdr->msgh_size,    // size of header + body
               0,                      // receive limit
               MACH_PORT_NULL,         // receive name
               MACH_MSG_TIMEOUT_NONE,  // no timeout, wait forever
               MACH_PORT_NULL);        // no notification port

  if (kr != KERN_SUCCESS)
    mach_port_deallocate(mach_task_self(), receiving_port);

  return kr;
}

::base::mac::ScopedMachSendRight ReceiveMachShmData(
    mach_port_t port_to_listen_on, std::string* data) {
  MachReceiveShmDataMessage recv_msg;
  mach_msg_header_t* recv_hdr = &recv_msg.header;
  recv_hdr->msgh_local_port = port_to_listen_on;
  recv_hdr->msgh_size = sizeof(recv_msg);

  kern_return_t kr =
      mach_msg(recv_hdr,               // message buffer
               MACH_RCV_MSG,           // option indicating service
               0,                      // send size
               recv_hdr->msgh_size,    // size of header + body
               port_to_listen_on,      // receive name
               MACH_MSG_TIMEOUT_NONE,  // no timeout, wait forever
               MACH_PORT_NULL);        // no notification port
  if (kr != KERN_SUCCESS)
    return ::base::mac::ScopedMachSendRight(MACH_PORT_NULL);
  if (recv_msg.header.msgh_id != 0)
    return ::base::mac::ScopedMachSendRight(MACH_PORT_NULL);
  *data = std::string(reinterpret_cast<const char*>(recv_msg.data.address),
                      recv_msg.data.size);
  return ::base::mac::ScopedMachSendRight(recv_msg.task_port.name);
}

PlatformHandleBroker::PlatformHandleBroker() = default;
PlatformHandleBroker::~PlatformHandleBroker() {
  if (broker_) {
    broker_->RemoveObserver(this);
  }
}

StatusOr<ChannelDef> PlatformHandleBroker::Setup(FillDataCallback callback) {
  DCHECK(!broker_);

  broker_ = std::make_unique<MachPortBroker>();
  Status s = broker_->Init();
  if (!s.ok()) {
    return s;
  }

  broker_->AddObserver(this);
  fill_data_callback_ = callback;

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_SHM);
  channel_def.mutable_shm_endpoint()
      ->mutable_broker_endpoint()
      ->set_service_name(broker_->service_name());
  return channel_def;
}

void PlatformHandleBroker::WaitForBroker(ChannelDef channel_def,
                                         ReceiveDataCallback callback) {
  DCHECK(!broker_);
  DCHECK(!callback.is_null());
  DCHECK(receive_data_callback_.is_null());

  std::string service_name =
      channel_def.shm_endpoint().broker_endpoint().service_name();
  ::base::mac::ScopedMachReceiveRight receiving_port(MakeReceivingPort());
  Status s =
      MachPortBroker::SendTaskPortToService(service_name, receiving_port.get());
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }

  Data data;
  ::base::mac::ScopedMachSendRight platform_handle =
      ReceiveMachShmData(receiving_port.get(), &data.data);
  if (platform_handle.get() == MACH_PORT_NULL) {
    std::move(callback).Run(
        errors::DataLoss("Failed to receive Mach SharedMemory Data."));
    return;
  }

  data.platform_handle = platform_handle.release();

  std::move(callback).Run(data);
}

void PlatformHandleBroker::OnReceivedTaskPort(::base::ProcessHandle process) {
  Data data;
  fill_data_callback_.Run(&data);

  mach_port_t mach_port = broker_->TaskForPid(process);
  kern_return_t kr = SendMachShmData(mach_port, data.platform_handle, data.data,
                                     MACH_MSG_TYPE_COPY_SEND);
  if (kr != KERN_SUCCESS) {
    LOG(ERROR) << "Failed to send Mach SharedMemory Data.";
  }

  broker_->GetLock().Acquire();
  broker_->InvalidatePid(process);
  broker_->GetLock().Release();
}

}  // namespace felicia