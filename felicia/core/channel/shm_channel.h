#ifndef FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/shared_memory/platform_handle_broker.h"
#include "felicia/core/channel/shared_memory/shared_memory.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class ShmChannel : public Channel<MessageTy> {
 public:
  ShmChannel(const channel::ShmSettings& settings = channel::ShmSettings());
  ~ShmChannel();

  bool IsShmChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_SHM;
  }

  bool HasReceivers() const override { return true; }

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  StatusOr<ChannelDef> MakeSharedMemory();

 private:
  void OnReceiveData(StatusOr<PlatformHandleBroker::Data> status_or);
  void FillData(PlatformHandleBroker::Data* handle_info);

  void ReadImpl(MessageTy* message, StatusOnceCallback callback) override;
  void OnReceiveMessageWithHeader(const Status& s);

  channel::ShmSettings settings_;
  PlatformHandleBroker broker_;
  StatusOnceCallback connect_callback_;

  DISALLOW_COPY_AND_ASSIGN(ShmChannel);
};

template <typename MessageTy>
ShmChannel<MessageTy>::ShmChannel(const channel::ShmSettings& settings)
    : settings_(settings) {}

template <typename MessageTy>
ShmChannel<MessageTy>::~ShmChannel() = default;

template <typename MessageTy>
void ShmChannel<MessageTy>::Connect(const ChannelDef& channel_def,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_impl_);
  DCHECK(!callback.is_null());

  connect_callback_ = std::move(callback);
  broker_.WaitForBroker(channel_def,
                        base::BindOnce(&ShmChannel<MessageTy>::OnReceiveData,
                                       base::Unretained(this)));
}

template <typename MessageTy>
StatusOr<ChannelDef> ShmChannel<MessageTy>::MakeSharedMemory() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ =
      std::make_unique<SharedMemory>(settings_.shm_size.bytes());

  return broker_.Setup(base::BindRepeating(&ShmChannel<MessageTy>::FillData,
                                           base::Unretained(this)));
}

template <typename MessageTy>
void ShmChannel<MessageTy>::OnReceiveData(
    StatusOr<PlatformHandleBroker::Data> status_or) {
  if (!status_or.ok()) {
    std::move(connect_callback_).Run(status_or.status());
    return;
  }

  PlatformHandleBroker::Data data = status_or.ValueOrDie();
  ChannelDef channel_def;
  if (!channel_def.ParseFromString(data.data)) {
    std::move(connect_callback_)
        .Run(errors::Unavailable("Failed to ParseFromString"));
    return;
  }

  ShmPlatformHandle* platform_handle =
      channel_def.mutable_shm_endpoint()->mutable_platform_handle();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  platform_handle->set_mach_port(static_cast<uint64_t>(data.platform_handle));
#elif defined(OS_WIN)
#else
  platform_handle->mutable_fd_pair()->set_fd(data.platform_handle.fd);
  platform_handle->mutable_fd_pair()->set_readonly_fd(
      data.platform_handle.readonly_fd);
#endif
  LOG(INFO) << channel_def.DebugString();
  this->channel_impl_ =
      std::unique_ptr<SharedMemory>(SharedMemory::FromChannelDef(channel_def));
  std::move(connect_callback_).Run(Status::OK());
}

template <typename MessageTy>
void ShmChannel<MessageTy>::FillData(PlatformHandleBroker::Data* handle_info) {
  SharedMemory* shared_memory = this->channel_impl_->ToSharedMemory();
  ChannelDef channel_def = shared_memory->ToChannelDef();
  handle_info->data = channel_def.SerializeAsString();
  ShmPlatformHandle platform_handle =
      channel_def.shm_endpoint().platform_handle();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  handle_info->platform_handle =
      static_cast<mach_port_t>(platform_handle.mach_port());
#elif defined(OS_WIN)
#else
  const FDPair& fd_pair = platform_handle.fd_pair();
  handle_info->platform_handle.fd = fd_pair.fd();
  handle_info->platform_handle.readonly_fd = fd_pair.readonly_fd();
#endif
}

template <typename MessageTy>
void ShmChannel<MessageTy>::ReadImpl(MessageTy* message,
                                     StatusOnceCallback callback) {
  SharedMemory* shared_memory = this->channel_impl_->ToSharedMemory();
  size_t buffer_size = shared_memory->BufferSize();
  this->receive_buffer_.SetEnoughCapacityIfDynamic(buffer_size);

  this->message_ = message;
  this->receive_callback_ = std::move(callback);
  this->channel_impl_->ReadAsync(
      this->receive_buffer_.buffer(), this->receive_buffer_.capacity(),
      base::BindOnce(&ShmChannel<MessageTy>::OnReceiveMessageWithHeader,
                     base::Unretained(this)));
}

template <typename MessageTy>
void ShmChannel<MessageTy>::OnReceiveMessageWithHeader(const Status& s) {
  if (!s.ok()) {
    std::move(this->receive_callback_).Run(s);
    return;
  }

  MessageIOError err = MessageIO::ParseHeaderFromBuffer(
      this->receive_buffer_.StartOfBuffer(), &this->header_);
  if (err != MessageIOError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss(MessageIOErrorToString(err)));
    return;
  }

  if (this->receive_buffer_.capacity() - sizeof(Header) <
      this->header_.size()) {
    std::move(this->receive_callback_)
        .Run(errors::Aborted(
            MessageIOErrorToString(MessageIOError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }

  err = MessageIO::ParseMessageFromBuffer(this->receive_buffer_.StartOfBuffer(),
                                          this->header_, true, this->message_);
  if (err != MessageIOError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse message from buffer."));
    return;
  }

  std::move(this->receive_callback_).Run(s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_