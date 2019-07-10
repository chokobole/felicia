#ifndef FELICIA_CORE_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_
#define FELICIA_CORE_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_

#include "third_party/chromium/base/memory/platform_shared_memory_region.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

#if defined(OS_MACOSX) && !defined(OS_IOS)
#include "felicia/core/channel/shared_memory/mach_port_broker.h"
#elif defined(OS_WIN)
#else
#include "felicia/core/channel/socket/unix_domain_server_socket.h"
#endif

namespace felicia {

#if defined(OS_MACOSX) && !defined(OS_IOS)
class PlatformHandleBroker : public ::base::PortProvider::Observer {
#else
class PlatformHandleBroker {
#endif
 public:
  static constexpr size_t kDataLen = 1024;

  struct Data {
    ::base::subtle::PlatformSharedMemoryRegion::PlatformHandle platform_handle;
    std::string data;
  };

  using FillDataCallback = ::base::RepeatingCallback<void(Data*)>;
  using ReceiveDataCallback = ::base::OnceCallback<void(StatusOr<Data>)>;

  PlatformHandleBroker();
  ~PlatformHandleBroker();

  StatusOr<ChannelDef> Setup(FillDataCallback callback);
  void WaitForBroker(ChannelDef channel_def, ReceiveDataCallback callback);

#if defined(OS_MACOSX) && !defined(OS_IOS)
  void OnReceivedTaskPort(::base::ProcessHandle process) override;
#endif

 private:
#if defined(OS_MACOSX) && !defined(OS_IOS)
#elif defined(OS_WIN)
#else
  void OnBrokerConnect(const Status& s);
  void AcceptLoop();
  void OnBrokerAccept(StatusOr<std::unique_ptr<::net::SocketPosix>> status_or);
  bool OnBrokerAuth(const UnixDomainServerSocket::Credentials& credentials);
#endif

#if defined(OS_MACOSX) && !defined(OS_IOS)
  std::unique_ptr<MachPortBroker> broker_;
#elif defined(OS_WIN)
#else
  std::unique_ptr<UnixDomainSocket> broker_;
#endif
  FillDataCallback fill_data_callback_;
  ReceiveDataCallback receive_data_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_