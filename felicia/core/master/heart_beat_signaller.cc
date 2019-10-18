#include "felicia/core/master/heart_beat_signaller.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/heart_beat_listener.h"

namespace felicia {

HeartBeatSignaller::HeartBeatSignaller()
    : thread_("HeartBeatSignallerThread") {}

void HeartBeatSignaller::Start(const ClientInfo& client_info,
                               OnStartCallback callback) {
  heart_beat_duration_ = GetHeartBeatDuration(client_info);

  thread_.StartWithOptions(
      base::Thread::Options{base::MessageLoop::TYPE_IO, 0});
  thread_.task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&HeartBeatSignaller::DoStart,
                                base::Unretained(this), std::move(callback)));
}

void HeartBeatSignaller::DoStart(OnStartCallback callback) {
  DCHECK(!channel_);

  channel_ =
      ChannelFactory::NewChannel<HeartBeat>(ChannelDef::CHANNEL_TYPE_TCP);

  channel_->SetSendBufferSize(kHeartBeatBytes);

  TCPChannel<HeartBeat>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  tcp_channel->AcceptLoop(base::BindRepeating(&HeartBeatSignaller::OnAccept,
                                              base::Unretained(this)));
  ChannelSource channel_source;
  *channel_source.add_channel_defs() = status_or.ValueOrDie();
  std::move(callback).Run(channel_source);
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

void HeartBeatSignaller::OnSignal(const Status& s) {
  if (s.ok() || trial_ <= kMaximumTrial) {
    thread_.task_runner()->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&HeartBeatSignaller::Signal, base::Unretained(this)),
        heart_beat_duration_);
  } else {
    LOG(ERROR) << "Failed to send heart...";
  }
}

}  // namespace felicia