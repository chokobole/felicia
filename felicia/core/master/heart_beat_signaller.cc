#include "felicia/core/master/heart_beat_signaller.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/heart_beat_listener.h"

namespace felicia {

namespace {

::base::TimeDelta g_heart_beat_duration = ::base::TimeDelta();

}  // namespace

HeartBeatSignaller::HeartBeatSignaller(
    TaskRunnerInterface* task_runner_interface)
    : task_runner_interface_(task_runner_interface) {}

void HeartBeatSignaller::Start() {
  DCHECK(!channel_);

  if (g_heart_beat_duration == ::base::TimeDelta()) {
    g_heart_beat_duration = GetHeartBeatDuration();
  }

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::TCP);
  channel_ = ChannelFactory::NewChannel<HeartBeat>(channel_def);

  TCPChannel<HeartBeat>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  channel_source_ = status_or.ValueOrDie();
  tcp_channel->DoAcceptLoop(::base::BindRepeating(&HeartBeatSignaller::OnAccept,
                                                  ::base::Unretained(this)));
}

void HeartBeatSignaller::OnAccept(const Status& s) {
  if (s.ok()) {
    Signal();
  } else {
    LOG(ERROR) << "Failed to accept: " << s.error_message();
  }
}

void HeartBeatSignaller::Signal() {
  trial_++;
  HeartBeat heart_beat;
  heart_beat.set_ok(true);
  channel_->SendMessage(heart_beat,
                        ::base::BindOnce(&HeartBeatSignaller::OnSignal,
                                         ::base::Unretained(this)));
}

void HeartBeatSignaller::OnSignal(const Status& s) {
  if (s.ok() || trial_ <= kMaximumTrial) {
    task_runner_interface_->PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&HeartBeatSignaller::Signal, ::base::Unretained(this)),
        g_heart_beat_duration);
  } else {
    LOG(ERROR) << "Failed to send heart...";
  }
}

}  // namespace felicia