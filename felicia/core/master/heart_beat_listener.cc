#include "felicia/core/master/heart_beat_listener.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

namespace {

static constexpr int64_t kDefeaultHeartBeatDuration = 1000;

base::TimeDelta g_heart_beat_duration = base::TimeDelta();

}  // namespace

base::TimeDelta GetHeartBeatDuration() {
  int64_t duration = kDefeaultHeartBeatDuration;
  const char* duration_str = getenv("FEL_HEART_BEAT_DURATION");
  if (duration_str) {
    if (base::StringToInt64(duration_str, &duration)) {
      if (duration < 0) {
        LOG(WARNING) << "Duration cannot be negative " << duration
                     << ", set to default value " << kDefeaultHeartBeatDuration;
        duration = kDefeaultHeartBeatDuration;
      }
    }
  }

  return base::TimeDelta::FromMilliseconds(duration);
}

HeartBeatListener::HeartBeatListener(const ClientInfo& client_info,
                                     OnDisconnectCallback callback)
    : client_info_(client_info), callback_(std::move(callback)) {
  DCHECK(!callback_.is_null());
  uint32_t heart_beat_duration = client_info_.heart_beat_duration();
  if (heart_beat_duration == 0) {
    if (g_heart_beat_duration == base::TimeDelta()) {
      g_heart_beat_duration = GetHeartBeatDuration();
    }
    heart_beat_duration_ = g_heart_beat_duration;
  } else {
    heart_beat_duration_ =
        base::TimeDelta::FromMilliseconds(heart_beat_duration);
  }
}

HeartBeatListener::~HeartBeatListener() {
  std::move(callback_).Run(client_info_);
}

void HeartBeatListener::StartCheckHeartBeat() {
  DCHECK_EQ(client_info_.heart_beat_signaller_source().channel_defs_size(), 1);
  DCHECK_EQ(client_info_.heart_beat_signaller_source().channel_defs(0).type(),
            ChannelDef::CHANNEL_TYPE_TCP);

  channel_ = ChannelFactory::NewChannel<HeartBeat>(
      client_info_.heart_beat_signaller_source().channel_defs(0).type());

  channel_->SetReceiveBufferSize(kHeartBeatBytes);

  channel_->Connect(client_info_.heart_beat_signaller_source().channel_defs(0),
                    base::BindOnce(&HeartBeatListener::DoCheckHeartBeat,
                                   base::Unretained(this)));
}

void HeartBeatListener::DoCheckHeartBeat(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << "Failed to Connect "
               << client_info_.heart_beat_signaller_source().DebugString();
    KillSelf();
    return;
  }

  TryReceiveHeartBeat();
}

void HeartBeatListener::TryReceiveHeartBeat() {
  if (timeout_.IsCancelled()) {
    timeout_.Reset(
        base::BindOnce(&HeartBeatListener::KillSelf, base::Unretained(this)));
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, timeout_.callback(), kMultiplier * heart_beat_duration_);
  }
  channel_->ReceiveMessage(
      &heart_beat_,
      base::BindOnce(&HeartBeatListener::OnAlive, base::Unretained(this)));
}

void HeartBeatListener::OnAlive(const Status& s) {
  if (s.ok()) {
    timeout_.Cancel();
  } else if (channel_->IsTCPChannel() &&
             !channel_->ToTCPChannel()->IsConnected()) {
    KillSelf();
    return;
  }

  base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&HeartBeatListener::TryReceiveHeartBeat,
                     base::Unretained(this)),
      heart_beat_duration_);
}

void HeartBeatListener::KillSelf() {
  timeout_.Cancel();
  channel_.reset();
  delete this;
}

}  // namespace felicia