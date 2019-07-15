#include "felicia/core/channel/shared_memory/named_pipe_server.h"

#include <windows.h>

// NOTE: This needs to be included *after* windows.h.
#include <sddl.h>

#include <synchapi.h>

#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

namespace felicia {

// This is taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/mojo/public/cpp/platform/named_platform_channel_win.cc#L21-L43

namespace {

// A DACL to grant:
// GA = Generic All
// access to:
// SY = LOCAL_SYSTEM
// BA = BUILTIN_ADMINISTRATORS
// OW = OWNER_RIGHTS
constexpr base::char16 kDefaultSecurityDescriptor[] =
    L"D:(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;OW)";

}  // namespace

// static
::base::string16 NamedPipeServer::GetPipeNameFromServiceName(
    const std::string& service_name) {
  return L"\\\\.\\pipe\\felicia." + ::base::UTF8ToUTF16(service_name);
}

// static
::base::win::ScopedHandle NamedPipeServer::CreateNamedPipe(
    const std::string& service_name) {
  PSECURITY_DESCRIPTOR security_desc = nullptr;
  ULONG security_desc_len = 0;
  PCHECK(::ConvertStringSecurityDescriptorToSecurityDescriptor(
      kDefaultSecurityDescriptor, SDDL_REVISION_1, &security_desc,
      &security_desc_len));
  std::unique_ptr<void, decltype(::LocalFree)*> p(security_desc, ::LocalFree);
  SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES),
                                             security_desc, FALSE};

  const DWORD kOpenMode =
      PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE;
  const DWORD kPipeMode =
      PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_REJECT_REMOTE_CLIENTS;

  ::base::string16 pipe_name = GetPipeNameFromServiceName(service_name);
  return ::base::win::ScopedHandle(
      ::CreateNamedPipeW(pipe_name.c_str(), kOpenMode, kPipeMode,
                         1,     // Max instances.
                         4096,  // Out buffer size.
                         4096,  // In buffer size.
                         5000,  // Timeout in milliseconds.
                         &security_attributes));
}

NamedPipeServer::NamedPipeServer(Delegate* delegate) : delegate_(delegate) {
  service_name_ =
      base::StringPrintf("%lu.%lu.%I64u", ::GetCurrentProcessId(),
                         ::GetCurrentThreadId(), base::RandUint64());

  pipe_handle_ = CreateNamedPipe(service_name_);
  memset(&connect_overlapped_, 0, sizeof(connect_overlapped_));
  memset(&write_overlapped_, 0, sizeof(write_overlapped_));
  connect_overlapped_.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  write_overlapped_.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

  ConnectToNewClient(false);
}

NamedPipeServer::~NamedPipeServer() {
  connect_watcher_.StopWatching();
  CloseHandle(connect_overlapped_.hEvent);
  CloseHandle(write_overlapped_.hEvent);
}

void NamedPipeServer::OnObjectSignaled(HANDLE object) {
  DWORD num_bytes;
  BOOL ok = GetOverlappedResult(pipe_handle_.Get(), &connect_overlapped_,
                                &num_bytes, FALSE);

  if (!ok) {
    PLOG(ERROR) << "Failed to GetOverlappedResult.";
    return;
  }

  delegate_->OnConnected();

  ConnectToNewClient(true);
}

void NamedPipeServer::ConnectToNewClient(bool disconnect) {
  if (disconnect) {
    if (!DisconnectNamedPipe(pipe_handle_.Get())) {
      PLOG(ERROR) << "Failed to DisconnectNamedPipe";
    }
  }
  if (!ResetEvent(connect_overlapped_.hEvent)) {
    PLOG(ERROR) << "Failed to ResetEvent";
  }

  connect_watcher_.StartWatchingOnce(connect_overlapped_.hEvent, this);
  if (ConnectNamedPipe(pipe_handle_.Get(), &connect_overlapped_)) {
    delegate_->OnConnected();
    ConnectToNewClient(true);
    return;
  }

  DWORD err = GetLastError();
  if (err == ERROR_IO_PENDING) {
  } else if (err == ERROR_PIPE_CONNECTED) {
    if (!SetEvent(connect_overlapped_.hEvent)) {
      PLOG(ERROR) << "Failed to SetEvent";
    }
  }
}

void NamedPipeServer::WriteFile(const std::string& data) {
  DWORD written = 0;
  while (true) {
    BOOL result = ::WriteFile(pipe_handle_.Get(), data.c_str(), data.length(),
                              &written, &write_overlapped_);
    if (result && written == data.length()) {
      break;
    }

    DWORD err = GetLastError();
    if (!result && err == ERROR_IO_PENDING) {
      break;
    }
  }
}

}  // namespace felicia