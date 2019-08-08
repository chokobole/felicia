#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_NAMED_PIPE_SERVER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_NAMED_PIPE_SERVER_H_

#include <windows.h>

#include "third_party/chromium/base/strings/string16.h"
#include "third_party/chromium/base/win/object_watcher.h"
#include "third_party/chromium/base/win/scoped_handle.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT NamedPipeServer : public base::win::ObjectWatcher::Delegate {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void OnConnected() = 0;
  };

  static base::string16 GetPipeNameFromServiceName(
      const std::string& service_name);
  static base::win::ScopedHandle CreateNamedPipe(
      const std::string& service_name);

  explicit NamedPipeServer(Delegate* delegate);
  ~NamedPipeServer();

  // base::ObjectWatcher::Delegate methods:
  void OnObjectSignaled(HANDLE object) override;

  const std::string& service_name() const { return service_name_; }

  void WriteFile(const std::string& data);

 private:
  void ConnectToNewClient(bool disconnect);

  Delegate* delegate_;

  base::win::ScopedHandle pipe_handle_;
  OVERLAPPED connect_overlapped_;
  OVERLAPPED write_overlapped_;

  base::win::ObjectWatcher connect_watcher_;

  std::string service_name_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_NAMED_PIPE_SERVER_H_