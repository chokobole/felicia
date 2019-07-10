#ifndef FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_

#include "third_party/chromium/build/build_config.h"
#if defined(OS_POSIX)
#include "third_party/chromium/base/posix/unix_domain_socket.h"
#endif

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/shared_memory/shared_memory.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/file/file_util.h"

namespace felicia {

template <typename MessageTy>
class ShmChannel : public Channel<MessageTy> {
 public:
  ShmChannel(const channel::ShmSettings& settings);
  ~ShmChannel();

  bool IsShmChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_SHM;
  }

  bool HasReceivers() const override { return true; }

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  StatusOr<ChannelDef> MakeReadOnlySharedMemory();

 private:
#if defined(OS_POSIX)
  void OnBrokerConnect(const Status& s);
  void AcceptLoop();
  void OnBrokerAccept(StatusOr<std::unique_ptr<::net::SocketPosix>> status_or);
  bool OnBrokerAuth(const UnixDomainServerSocket::Credentials& credentials);
#endif

  void ReadImpl(MessageTy* message, StatusOnceCallback callback) override;
  void OnReceiveMessageWithHeader(const Status& s);

  channel::ShmSettings settings_;
  std::unique_ptr<ChannelImpl> broker_;
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

#if defined(OS_POSIX)
  ::net::UDSEndPoint uds_endpoint;
  Status s = ToNetUDSEndPoint(channel_def, &uds_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  connect_callback_ = std::move(callback);
  broker_ = std::make_unique<UnixDomainClientSocket>();
  UnixDomainClientSocket* client_socket =
      broker_->ToSocket()->ToUnixDomainSocket()->ToUnixDomainClientSocket();
  client_socket->Connect(
      uds_endpoint, ::base::BindOnce(&ShmChannel<MessageTy>::OnBrokerConnect,
                                     ::base::Unretained(this)));
#endif
}

template <typename MessageTy>
StatusOr<ChannelDef> ShmChannel<MessageTy>::MakeReadOnlySharedMemory() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ =
      std::make_unique<SharedMemory>(settings_.shm_size.bytes());

#if defined(OS_POSIX)
  broker_ = std::make_unique<UnixDomainServerSocket>();
  UnixDomainServerSocket* server_socket =
      broker_->ToSocket()->ToUnixDomainSocket()->ToUnixDomainServerSocket();
  auto status_or = server_socket->BindAndListen();
  if (status_or.ok()) {
    status_or.ValueOrDie().set_type(ChannelDef::CHANNEL_TYPE_SHM);
    AcceptLoop();
  }

  return status_or;
#else
  return errors::Unimplemented("");
#endif
}

#if defined(OS_POSIX)
template <typename MessageTy>
void ShmChannel<MessageTy>::OnBrokerConnect(const Status& s) {
  if (!s.ok()) {
    std::move(connect_callback_).Run(s);
    return;
  }

  UnixDomainClientSocket* client_socket =
      broker_->ToSocket()->ToUnixDomainSocket()->ToUnixDomainClientSocket();

  size_t len = 1024;
  char buf[len];
  std::vector<::base::ScopedFD> fds;
  int socket_fd = client_socket->socket_fd();
  if (!SetBlocking(socket_fd, true)) {
    PLOG(ERROR) << "Failed to SetBlocking";
  }
  ssize_t read = ::base::UnixDomainSocket::RecvMsg(socket_fd, buf, len, &fds);
  if (read < 0) {
    PLOG(ERROR) << "Failed to RecvMsg";
    std::move(connect_callback_).Run(errors::Unavailable("Failed to RecvMsg"));
    return;
  }

  std::string txt(buf, read);
  ChannelDef channel_def;
  if (!channel_def.ParseFromArray(buf, read)) {
    std::move(connect_callback_)
        .Run(errors::Unavailable("Failed to ParseFromArray"));
    return;
  }
  channel_def.mutable_shm_endpoint()
      ->mutable_platform_handle()
      ->mutable_fd_pair()
      ->set_fd(fds[0].release());
  LOG(INFO) << channel_def.DebugString();
  this->channel_impl_ =
      std::unique_ptr<SharedMemory>(SharedMemory::FromChannelDef(channel_def));
  std::move(connect_callback_).Run(Status::OK());
}

template <typename MessageTy>
void ShmChannel<MessageTy>::AcceptLoop() {
  UnixDomainServerSocket* server_socket =
      broker_->ToSocket()->ToUnixDomainSocket()->ToUnixDomainServerSocket();
  server_socket->AcceptOnceIntercept(
      ::base::BindOnce(&ShmChannel<MessageTy>::OnBrokerAccept,
                       ::base::Unretained(this)),
      ::base::BindRepeating(&ShmChannel<MessageTy>::OnBrokerAuth,
                            ::base::Unretained(this)));
}

template <typename MessageTy>
void ShmChannel<MessageTy>::OnBrokerAccept(
    StatusOr<std::unique_ptr<::net::SocketPosix>> status_or) {
  if (status_or.ok()) {
    SharedMemory* shared_memory = this->channel_impl_->ToSharedMemory();
    ChannelDef channel_def = shared_memory->ToChannelDef();
    std::string txt = channel_def.SerializeAsString();
    std::vector<int> fds;
    const FDPair& fd_pair =
        channel_def.shm_endpoint().platform_handle().fd_pair();
    fds.push_back(fd_pair.fd());
    if (fd_pair.readonly_fd() != -1) fds.push_back(fd_pair.readonly_fd());
    int socket_fd = status_or.ValueOrDie()->socket_fd();
    if (!SetBlocking(socket_fd, true)) {
      PLOG(ERROR) << "Failed to SetBlocking";
    }
    if (!::base::UnixDomainSocket::SendMsg(socket_fd, txt.c_str(), txt.length(),
                                           fds)) {
      PLOG(ERROR) << "Failed to SendMsg";
    } else {
      LOG(INFO) << "Succeed to SendMsg";
    }
  }
  AcceptLoop();
}

template <typename MessageTy>
bool ShmChannel<MessageTy>::OnBrokerAuth(
    const UnixDomainServerSocket::Credentials& credentials) {
  return true;
}
#endif

template <typename MessageTy>
void ShmChannel<MessageTy>::ReadImpl(MessageTy* message,
                                     StatusOnceCallback callback) {
  if (this->is_dynamic_buffer_ && this->receive_buffer_->capacity() == 0) {
    SharedMemory* shared_memory = this->channel_impl_->ToSharedMemory();
    this->receive_buffer_->SetCapacity(shared_memory->BufferSize());
  }

  this->message_ = message;
  this->receive_callback_ = std::move(callback);
  this->channel_impl_->Read(
      this->receive_buffer_, this->receive_buffer_->capacity(),
      ::base::BindOnce(&ShmChannel<MessageTy>::OnReceiveMessageWithHeader,
                       ::base::Unretained(this)));
}

template <typename MessageTy>
void ShmChannel<MessageTy>::OnReceiveMessageWithHeader(const Status& s) {
  if (!s.ok()) {
    std::move(this->receive_callback_).Run(s);
    return;
  }

  MessageIoError err = MessageIO<MessageTy>::ParseHeaderFromBuffer(
      this->receive_buffer_->StartOfBuffer(), &this->header_);
  if (err != MessageIoError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss(MessageIoErrorToString(err)));
    return;
  }

  if (this->receive_buffer_->capacity() - sizeof(Header) <
      this->header_.size()) {
    std::move(this->receive_callback_)
        .Run(errors::Aborted(
            MessageIoErrorToString(MessageIoError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }

  err = MessageIO<MessageTy>::ParseMessageFromBuffer(
      this->receive_buffer_->StartOfBuffer(), this->header_, true,
      this->message_);
  if (err != MessageIoError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse message from buffer."));
    return;
  }

  std::move(this->receive_callback_).Run(s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHM_CHANNEL_H_