#include "felicia/core/communication/master_proxy.h"

#include <unistd.h>

#include "absl/time/time.h"

#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/run_loop.h"

namespace felicia {

namespace {

felicia::RunLoop* g_run_loop = nullptr;

}  // namespace

MasterProxy::MasterProxy() {
  message_loop_ = absl::make_unique<MessageLoop>();
  g_run_loop = new RunLoop();
}

MasterProxy& MasterProxy::GetInstance() {
  static NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

void MasterProxy::Spin() { g_run_loop->Run(); }

}  // namespace felicia