#include "felicia/core/lib/base/run_loop.h"

#include "felicia/core/lib/base/no_destructor.h"

namespace felicia {

namespace {

RunLoop::Delegate** GetDelegate() {
  static thread_local NoDestructor<RunLoop::Delegate*> delegate;
  return delegate.get();
}

}  // namespace

RunLoop::Delegate::Delegate() = default;

RunLoop::Delegate::~Delegate() {
  // A RunLoop::Delegate may be destroyed before it is bound, if so it may still
  // be on its creation thread (e.g. a Thread that fails to start) and
  // shouldn't disrupt that thread's state.
  if (bound_) *GetDelegate() = nullptr;
}

// static
void RunLoop::RegisterDelegateForCurrentThread(Delegate* delegate) {
  // Bind |delegate| to this thread.
  DCHECK(!delegate->bound_);

  // There can only be one RunLoop::Delegate per thread.
  DCHECK(!(*GetDelegate())) << "Error: Multiple RunLoop::Delegates registered "
                               "on the same thread.\n\n";
  *GetDelegate() = delegate;
  delegate->bound_ = true;
}

RunLoop::RunLoop() : delegate_(*GetDelegate()) { DCHECK(delegate_); }

RunLoop::~RunLoop() = default;

void RunLoop::Run() {
  if (!BeforeRun()) return;

  delegate_->Run();

  AfterRun();
}

void RunLoop::Quit() {
  quit_called_ = true;
  if (running_) {
    delegate_->Quit();
  }
}

bool RunLoop::BeforeRun() {
#if DCHECK_IS_ON()
  DCHECK(!run_called_);
  run_called_ = true;
#endif  // DCHECK_IS_ON()

  // Allow Quit to be called before Run.
  if (quit_called_) return false;

  running_ = true;
  return true;
}

void RunLoop::AfterRun() { running_ = false; }

}  // namespace felicia