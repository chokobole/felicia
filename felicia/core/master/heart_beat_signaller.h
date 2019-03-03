#ifndef FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_
#define FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/master/task_runner_interface.h"

namespace felicia {

class HeartBeatSignaller {
 public:
  explicit HeartBeatSignaller(TaskRunnerInterface* task_runner_interface);

  const ChannelSource& channel_source() const { return channel_source_; }

  void Start();

 private:
  void OnListen(::base::WaitableEvent* event,
                const StatusOr<ChannelSource>& status_or);

  void OnAccept(const Status& s);
  void Signal();
  void OnSignal(const Status& s);

  TaskRunnerInterface* task_runner_interface_;  // not owned
  ChannelSource channel_source_;
  std::unique_ptr<Channel<HeartBeat>> channel_;

  uint8_t trial_ = 0;
  static constexpr uint8_t kMaximumTrial = 5;

  DISALLOW_COPY_AND_ASSIGN(HeartBeatSignaller);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_