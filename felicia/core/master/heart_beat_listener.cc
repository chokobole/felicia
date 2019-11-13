// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/heart_beat_listener.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/heart_beat_signaller.h"

namespace felicia {

HeartBeatListener::HeartBeatListener(const ClientInfo& client_info,
                                     OnDisconnectCallback callback)
    : client_info_(client_info),
      heart_beat_duration_(GetHeartBeatDuration(client_info)),
      callback_(std::move(callback)) {
  DCHECK(!callback_.is_null());
}

HeartBeatListener::~HeartBeatListener() {
  std::move(callback_).Run(client_info_);
}

void HeartBeatListener::StartCheckHeartBeat() {
  DCHECK_EQ(client_info_.heart_beat_signaller_source().channel_defs_size(), 1);
  DCHECK_EQ(client_info_.heart_beat_signaller_source().channel_defs(0).type(),
            ChannelDef::CHANNEL_TYPE_TCP);

  channel_ = ChannelFactory::NewChannel(
      client_info_.heart_beat_signaller_source().channel_defs(0).type());

  channel_->SetReceiveBufferSize(kHeartBeatBytes);

  channel_->Connect(client_info_.heart_beat_signaller_source().channel_defs(0),
                    base::BindOnce(&HeartBeatListener::DoCheckHeartBeat,
                                   base::Unretained(this)));
}

void HeartBeatListener::DoCheckHeartBeat(Status s) {
  if (!s.ok()) {
    LOG(ERROR) << "Failed to Connect "
               << client_info_.heart_beat_signaller_source().DebugString();
    KillSelf();
    return;
  }

  receiver_.set_channel(channel_.get());
  TryReceiveHeartBeat();
}

void HeartBeatListener::TryReceiveHeartBeat() {
  if (stopped_) {
    return;
  }
  if (timeout_.IsCancelled()) {
    timeout_.Reset(base::BindOnce(&HeartBeatListener::StopCheckHeartBeat,
                                  base::Unretained(this)));
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, timeout_.callback(), kMultiplier * heart_beat_duration_);
  }
  receiver_.ReceiveMessage(
      base::BindOnce(&HeartBeatListener::OnAlive, base::Unretained(this)));
}

void HeartBeatListener::OnAlive(Status s) {
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

void HeartBeatListener::StopCheckHeartBeat() {
  stopped_ = true;
  base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&HeartBeatListener::KillSelf, base::Unretained(this)),
      heart_beat_duration_);
}

void HeartBeatListener::KillSelf() {
  timeout_.Cancel();
  channel_.reset();
  delete this;
}

}  // namespace felicia