// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_HEART_BEAT_LISTENER_H_
#define FELICIA_CORE_MASTER_HEART_BEAT_LISTENER_H_

#include <memory>

#include "third_party/chromium/base/cancelable_callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/master/bytes_constants.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class HeartBeatListener {
 public:
  using OnDisconnectCallback =
      base::OnceCallback<void(const ClientInfo& client_info)>;

  explicit HeartBeatListener(const ClientInfo& client_info,
                             OnDisconnectCallback callback);

  // Connect to client and read the |HeartBeat| periodically.
  void StartCheckHeartBeat();

 private:
  ~HeartBeatListener();

  void DoCheckHeartBeat(Status s);

  void TryReceiveHeartBeat();

  // If |HeartBeat| is arrived right at the time, then it repeats to call
  // |DoCheckHeart| method. During a certain amount of duration, if heart
  // beat message can't be reached, then listener regard it as a dead one.
  void OnAlive(Status s);

  void StopCheckHeartBeat();

  void KillSelf();

  ClientInfo client_info_;
  base::TimeDelta heart_beat_duration_;
  OnDisconnectCallback callback_;
  std::unique_ptr<Channel> channel_;
  MessageReceiver<HeartBeat> receiver_;

  base::CancelableOnceClosure timeout_;
  bool stopped_ = false;
  static constexpr uint8_t kMultiplier = 5;

  DISALLOW_COPY_AND_ASSIGN(HeartBeatListener);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_HEART_BEAT_LISTENER_H_