#include "felicia/core/master/heart_beat_signaller.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/heart_beat_listener.h"

namespace felicia {

namespace {

base::TimeDelta g_heart_beat_duration = base::TimeDelta();

}  // namespace

HeartBeatSignaller::HeartBeatSignaller(
    TaskRunnerInterface* task_runner_interface)
    : task_runner_interface_(task_runner_interface) {}

void HeartBeatSignaller::Start() {
  DCHECK(!channel_);

  if (g_heart_beat_duration == base::TimeDelta()) {
    g_heart_beat_duration = GetHeartBeatDuration();
  }

  channel_ =
      ChannelFactory::NewChannel<HeartBeat>(ChannelDef::CHANNEL_TYPE_TCP);

  channel_->SetSendBufferSize(kHeartBeatBytes);

  TCPChannel<HeartBeat>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  *channel_source_.add_channel_defs() = status_or.ValueOrDie();
  AcceptLoop();
}

void HeartBeatSignaller::AcceptLoop() {
  if (!task_runner_interface_->IsBoundToCurrentThread()) {
    task_runner_interface_->PostTask(
        FROM_HERE, base::BindOnce(&HeartBeatSignaller::AcceptLoop,
                                  base::Unretained(this)));
    return;
  }
  TCPChannel<HeartBeat>* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->AcceptLoop(base::BindRepeating(&HeartBeatSignaller::OnAccept,
                                              base::Unretained(this)));
}

void HeartBeatSignaller::OnAccept(const Status& s) {
  if (s.ok()) {
    Signal();
  } else {
    LOG(ERROR) << "Failed to accept: " << s;
  }
}

void HeartBeatSignaller::Signal() {
  trial_++;
  HeartBeat heart_beat;
  heart_beat.set_ok(true);
  channel_->SendMessage(heart_beat,
                        base::BindRepeating(&HeartBeatSignaller::OnSignal,
                                            base::Unretained(this)));
}

void HeartBeatSignaller::OnSignal(ChannelDef::Type type, const Status& s) {
  if (s.ok() || trial_ <= kMaximumTrial) {
    task_runner_interface_->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&HeartBeatSignaller::Signal, base::Unretained(this)),
        g_heart_beat_duration);
  } else {
    LOG(ERROR) << "Failed to send heart...";
  }
}

}  // namespace felicia