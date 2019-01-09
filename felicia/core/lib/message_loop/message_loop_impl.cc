#include "felicia/core/lib/message_loop/message_loop_impl.h"

#include "felicia/core/lib/message_loop/message_loop_current.h"
#include "felicia/core/platform/platform_thread.h"

namespace felicia {

MessageLoopImpl::MessageLoopImpl() = default;

MessageLoopImpl::~MessageLoopImpl() = default;

bool MessageLoopImpl::IsBoundToCurrentThread() const {
  return MessageLoopCurrent::Get()->IsBoundToCurrentThread();
}

MessagePump* MessageLoopImpl::GetMessagePump() const { return pump_.get(); }

void MessageLoopImpl::BindToCurrentThread(std::unique_ptr<MessagePump> pump) {
  DCHECK(!pump_);
  pump_ = std::move(pump);

  thread_id_ = PlatformThread::CurrentId();

  RunLoop::RegisterDelegateForCurrentThread(this);
  MessageLoopCurrent::BindToCurrentThreadInternal(this);
}

void MessageLoopImpl::Run() { pump_->Run(this); }

void MessageLoopImpl::Quit() { pump_->Quit(); }

void MessageLoopImpl::EnsureWorkScheduled() {
  // if (sequenced_task_source_->HasTasks())
  //   pump_->ScheduleWork();
}

bool MessageLoopImpl::DoWork() {
  // Nothing happened.
  return false;
}

bool MessageLoopImpl::DoDelayedWork(absl::Time* next_delayed_work_time) {
  return false;
}

bool MessageLoopImpl::DoIdleWork() { return false; }

}  // namespace felicia