#include "felicia/core/master/node_initializer.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

NodeInitializer::NodeInitializer(std::unique_ptr<NodeLifecycle> node_lifecycle)
    : node_lifecycle_(std::move(node_lifecycle)),
      thread_("InitializerThread") {}

void NodeInitializer::Initialize(OnInitCallback callback) {
  DCHECK(callback_.is_null());
  DCHECK(!callback.is_null());
  callback_ = std::move(callback);
  thread_.Start();
  thread_.task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&NodeInitializer::DoInitialize, base::Unretained(this)));
}

void NodeInitializer::DoInitialize() {
  node_lifecycle_->OnInit();
  std::move(callback_).Run(std::move(node_lifecycle_));
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.PostTask(FROM_HERE, base::BindOnce(&NodeInitializer::ReleaseSelf,
                                                  base::Unretained(this)));
}

void NodeInitializer::ReleaseSelf() { delete this; }

}  // namespace felicia