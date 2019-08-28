#include "felicia/core/channel/shared_memory/shared_memory.h"

#include "felicia/core/channel/shared_memory/read_only_shared_buffer.h"
#include "felicia/core/channel/shared_memory/writable_shared_buffer.h"
#include "felicia/core/lib/error/errors.h"

namespace felicia {

SharedMemory::SharedMemory(size_t size)
    : buffer_(std::make_unique<WritableSharedBuffer>(size)) {}

SharedMemory::SharedMemory(base::subtle::PlatformSharedMemoryRegion handle)
    : buffer_(std::make_unique<ReadOnlySharedBuffer>(std::move(handle))) {}

SharedMemory::~SharedMemory() = default;

void SharedMemory::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                              StatusOnceCallback callback) {
  WritableSharedBuffer* writable_buffer = buffer_->ToWritableSharedBuffer();
  if (static_cast<size_t>(size) > writable_buffer->size()) {
    std::move(callback).Run(errors::OutOfRange("Buffer size is not enough."));
    return;
  }

  writable_buffer->WriteBegin();
  memcpy(writable_buffer->data(), buffer->data(), size);
  writable_buffer->WriteEnd();

  std::move(callback).Run(Status::OK());
}

void SharedMemory::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                             int size, StatusOnceCallback callback) {
  ReadOnlySharedBuffer* readonly_buffer = buffer_->ToReadOnlySharedBuffer();
  const int kMaximumContentionCount = 10;
  int contention_count = -1;
  base::subtle::Atomic32 version;
  do {
    version = readonly_buffer->ReadBegin();
    if (last_version_ != version) {
      memcpy(buffer->data(), readonly_buffer->data(), size);
    }

    ++contention_count;
    if (contention_count == kMaximumContentionCount) break;
  } while (readonly_buffer->ReadRetry(version) || last_version_ == version);

  if (contention_count == kMaximumContentionCount) {
    std::move(callback).Run(
        errors::Unavailable("Reached to maximum contention count."));
  } else {
    last_version_ = version;
    std::move(callback).Run(Status::OK());
  }
}

size_t SharedMemory::BufferSize() const {
  if (!buffer_) return 0;

  if (buffer_->IsReadOnlySharedBuffer()) {
    ReadOnlySharedBuffer* readonly_buffer = buffer_->ToReadOnlySharedBuffer();
    return readonly_buffer->size();
  } else {
    WritableSharedBuffer* writable_buffer = buffer_->ToWritableSharedBuffer();
    return writable_buffer->size();
  }
}

ChannelDef SharedMemory::ToChannelDef() const {
  WritableSharedBuffer* writable_buffer = buffer_->ToWritableSharedBuffer();
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_SHM);
  base::subtle::PlatformSharedMemoryRegion region =
      base::ReadOnlySharedMemoryRegion::TakeHandleForSerialization(
          writable_buffer->DuplicateSharedMemoryRegion());
  base::subtle::PlatformSharedMemoryRegion::ScopedPlatformHandle
      platform_handle = region.PassPlatformHandle();
  base::UnguessableToken guid = region.GetGUID();
  base::subtle::PlatformSharedMemoryRegion::Mode mode = region.GetMode();
  size_t size = region.GetSize();

  ShmEndPoint* endpoint = channel_def.mutable_shm_endpoint();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  endpoint->mutable_platform_handle()->set_mach_port(
      static_cast<uint64_t>(platform_handle.release()));
#elif defined(OS_WIN)
  HandleWithProcessId* handle_with_process_id =
      endpoint->mutable_platform_handle()->mutable_handle_with_process_id();
  handle_with_process_id->set_handle(
      static_cast<uint64_t>(HandleToLong(platform_handle.Take())));
  handle_with_process_id->set_process_id(
      static_cast<uint64_t>(base::GetCurrentProcId()));
#else
  FDPair* fd_pair = endpoint->mutable_platform_handle()->mutable_fd_pair();
  fd_pair->set_fd(platform_handle.fd.release());
  fd_pair->set_readonly_fd(platform_handle.readonly_fd.release());
#endif
  endpoint->set_mode(static_cast<ShmEndPoint::Mode>(mode));
  endpoint->set_size(size);
  UngeussableToken* token = endpoint->mutable_guid();
  token->set_high(guid.GetHighForSerialization());
  token->set_low(guid.GetLowForSerialization());

  return channel_def;
}

// static
std::unique_ptr<SharedMemory> SharedMemory::FromChannelDef(
    ChannelDef channel_def) {
  base::subtle::PlatformSharedMemoryRegion::ScopedPlatformHandle
      scoped_platform_handle;
  const ShmEndPoint& endpoint = channel_def.shm_endpoint();
  auto mode = static_cast<base::subtle::PlatformSharedMemoryRegion::Mode>(
      endpoint.mode());
  auto guid = base::UnguessableToken::Deserialize(endpoint.guid().high(),
                                                  endpoint.guid().low());
  size_t size = endpoint.size();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  scoped_platform_handle = base::mac::ScopedMachSendRight{
      static_cast<mach_port_t>(endpoint.platform_handle().mach_port())};
#elif defined(OS_WIN)
  const HandleWithProcessId& handle_with_process_id =
      endpoint.platform_handle().handle_with_process_id();
  base::win::ScopedHandle remote_process_handle(
      ::OpenProcess(PROCESS_ALL_ACCESS, FALSE,
                    static_cast<DWORD>(handle_with_process_id.process_id())));
  HANDLE duped_handle;
  BOOL result = ::DuplicateHandle(remote_process_handle.Get(),
                                  LongToHandle(handle_with_process_id.handle()),
                                  GetCurrentProcess(), &duped_handle, 0, FALSE,
                                  DUPLICATE_SAME_ACCESS);
  if (result) {
    scoped_platform_handle = base::win::ScopedHandle(duped_handle);
  } else {
    PLOG(ERROR) << "Failed to DuplicateHandle.";
  }
#else
  const FDPair& fd_pair = endpoint.platform_handle().fd_pair();

  scoped_platform_handle = base::subtle::ScopedFDPair{
      base::ScopedFD{fd_pair.fd()}, base::ScopedFD{fd_pair.readonly_fd()}};
#endif

  return std::make_unique<SharedMemory>(
      base::subtle::PlatformSharedMemoryRegion::Take(
          std::move(scoped_platform_handle), mode, size, guid));
}

}  // namespace felicia