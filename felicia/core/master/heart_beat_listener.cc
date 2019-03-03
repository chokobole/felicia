#include "felicia/core/master/heart_beat_listener.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

::base::TimeDelta GetHeartBeatDuration() {
  int64_t duration = 1;
  const char* duration_str = getenv("FEL_HEART_BEAT_DURATION");
  if (duration_str) {
    if (::base::StringToInt64(duration_str, &duration)) {
      if (duration < 0) {
        LOG(WARNING) << "Duration cannot be negative " << duration
                     << ", set to default value 1";
        duration = 1;
      }
    }
  }

  return ::base::TimeDelta::FromSeconds(duration);
}

namespace {

::base::TimeDelta g_heart_beat_duration = ::base::TimeDelta();

}  // namespace

HeartBeatListener::HeartBeatListener(const ClientInfo& client_info,
                                     OnDisconnectCallback callback)
    : client_info_(client_info), callback_(std::move(callback)) {
  DCHECK(!callback_.is_null());
}

HeartBeatListener::~HeartBeatListener() {
  std::move(callback_).Run(client_info_);
}

void HeartBeatListener::StartCheckHeart() {
  if (g_heart_beat_duration == ::base::TimeDelta()) {
    g_heart_beat_duration = GetHeartBeatDuration();
  }

  channel_ = ChannelFactory::NewChannel<HeartBeat>(
      client_info_.heart_beat_signaller_source().channel_def());

  channel_->Connect(client_info_.heart_beat_signaller_source(),
                    ::base::BindOnce(&HeartBeatListener::DoCheckHeart,
                                     ::base::Unretained(this)));
}

void HeartBeatListener::DoCheckHeart(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << "Failed to Connect "
               << client_info_.heart_beat_signaller_source().DebugString();
    delete this;
    return;
  }

  trial_ = 0;
  TryReceiveHeartBeat();
}

void HeartBeatListener::TryReceiveHeartBeat() {
  trial_++;
  channel_->ReceiveMessage(
      &heart_beat_,
      ::base::BindOnce(&HeartBeatListener::OnAlive, ::base::Unretained(this)));
}

void HeartBeatListener::OnAlive(const Status& s) {
  if (s.ok()) {
    DoCheckHeart(Status::OK());
  } else {
    if (trial_ == kMaximumTrial) {
      delete this;
      return;
    }
    ::base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&HeartBeatListener::TryReceiveHeartBeat,
                         ::base::Unretained(this)),
        g_heart_beat_duration);
  }
}

}  // namespace felicia