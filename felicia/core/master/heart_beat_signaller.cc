// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/heart_beat_signaller.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/channel/message_sender.h"

namespace felicia {

namespace {

static constexpr int64_t kDefeaultHeartBeatDuration = 1000;

base::TimeDelta g_heart_beat_duration = base::TimeDelta();

}  // namespace

base::TimeDelta GetHeartBeatDuration(const ClientInfo& client_info) {
  uint32_t heart_beat_duration = client_info.heart_beat_duration();
  if (heart_beat_duration != 0)
    return base::TimeDelta::FromMilliseconds(heart_beat_duration);
  if (g_heart_beat_duration != base::TimeDelta()) return g_heart_beat_duration;

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

  g_heart_beat_duration = base::TimeDelta::FromMilliseconds(duration);
  return g_heart_beat_duration;
}

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

  channel_ = ChannelFactory::NewChannel(ChannelDef::CHANNEL_TYPE_TCP);

  channel_->SetSendBufferSize(kHeartBeatBytes);

  TCPChannel* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  tcp_channel->AcceptLoop(base::BindRepeating(&HeartBeatSignaller::OnAccept,
                                              base::Unretained(this)));
  ChannelSource channel_source;
  *channel_source.add_channel_defs() = status_or.ValueOrDie();
  std::move(callback).Run(channel_source);
}

void HeartBeatSignaller::OnAccept(Status s) {
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
  MessageSender<HeartBeat> sender(channel_.get());
  sender.SendMessage(heart_beat, base::BindOnce(&HeartBeatSignaller::OnSignal,
                                                base::Unretained(this)));
}

void HeartBeatSignaller::OnSignal(Status s) {
  if (s.ok() || trial_ <= kMaximumTrial) {
    thread_.task_runner()->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&HeartBeatSignaller::Signal, base::Unretained(this)),
        heart_beat_duration_);
  } else {
    LOG(ERROR) << "Failed to send heart: " << s;
  }
}

}  // namespace felicia