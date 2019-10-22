#ifndef FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_
#define FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/threading/thread.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class HeartBeatSignaller {
 public:
  typedef base::OnceCallback<void(const ChannelSource&)> OnStartCallback;

  HeartBeatSignaller();

  void Start(const ClientInfo& client_info, OnStartCallback callback);

 private:
  void DoStart(OnStartCallback callback);
  void AcceptLoop();
  void OnAccept(Status s);
  void Signal();
  void OnSignal(Status s);

  base::Thread thread_;
  base::TimeDelta heart_beat_duration_;
  std::unique_ptr<Channel> channel_;

  uint8_t trial_ = 0;
  static constexpr uint8_t kMaximumTrial = 5;

  DISALLOW_COPY_AND_ASSIGN(HeartBeatSignaller);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_HEART_BEAT_SIGNALLER_H_