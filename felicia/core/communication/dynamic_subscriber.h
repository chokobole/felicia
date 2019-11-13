// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_
#define FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"

namespace felicia {

class DynamicSubscriber : public Subscriber<DynamicProtobufMessage> {
 public:
  DynamicSubscriber();
  ~DynamicSubscriber();

  void Subscribe(const communication::Settings& settings,
                 OnMessageCallback on_message_callback,
                 StatusCallback on_error_callback = StatusCallback());

  void OnFindPublisher(const TopicInfo& topic_info);

  void Unsubscribe(const std::string& topic,
                   StatusOnceCallback callback = StatusCallback());

  const TopicInfo& topic_info() const { return topic_info_; }

 private:
  bool MaybeResolveMessgaeType(const TopicInfo& topic_info) override;

  DISALLOW_COPY_AND_ASSIGN(DynamicSubscriber);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_DYNAMIC_SUBSCRIBIER_H_