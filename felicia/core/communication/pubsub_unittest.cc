// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/threading/platform_thread.h"

#include "felicia/core/communication/publisher.h"
#include "felicia/core/communication/subscriber.h"
#include "felicia/core/lib/test/async_checker.h"
#include "felicia/core/message/test/simple_message.pb.h"
#include "felicia/core/util/timestamp/timestamper.h"

namespace felicia {

using namespace test;

namespace {

class MessageGenerator {
 public:
  SimpleMessage GenerateMessage() {
    SimpleMessage message;
    message.set_data(
        static_cast<int>(base::RandGenerator(std::numeric_limits<int>::max())));
    message.set_timestamp(timestamper_.timestamp().InMilliseconds());
    return message;
  }

 private:
  Timestamper timestamper_;
};

class MessageChecker : public AsyncChecker {
 public:
  void set_expected(const SimpleMessage& expected) { expected_ = expected; }

  void CheckMessage(SimpleMessage&& message) {
    EXPECT_EQ(expected_.data(), message.data());
    EXPECT_EQ(expected_.timestamp(), message.timestamp());
    CountDownTest();
  }

 private:
  SimpleMessage expected_;
};

}  // namespace

class PubSubTest : public testing::Test {
 public:
  PubSubTest() : topic_("message") {}

  void RequestPublish(int channel_types,
                      const communication::Settings& settings) {
    publisher_.RequestPublishForTesting(topic_, channel_types, settings);
  }

  void RequestSubscribe(
      int channel_types, const communication::Settings& settings,
      Subscriber<SimpleMessage>::OnMessageCallback message_callback) {
    subscriber_.RequestSubscribeForTesting(topic_, channel_types, settings,
                                           message_callback);
  }

  void NotifySubscriber() {
    subscriber_.OnFindPublisher(publisher_.topic_info_);
  }

  void Publish(const SimpleMessage& message) { publisher_.Publish(message); }

  SimpleMessage GenerateMessage() { return generator_.GenerateMessage(); }

  void Release() {
    publisher_.RequestUnpublishForTesting(topic_);
    subscriber_.RequestUnsubscribeForTesting(topic_);
  }

 protected:
  void SetUp() override {
    MainThread::SetBackground();
    MainThread::GetInstance().RunBackground();
  }

  std::string topic_;
  Publisher<SimpleMessage> publisher_;
  Subscriber<SimpleMessage> subscriber_;
  MessageGenerator generator_;
};

void SetupPubSub(PubSubTest* test, int channel_type, MessageChecker* checker) {
  communication::Settings settings;
  Bytes size = Bytes::FromBytes(512);
  settings.buffer_size = size;
  settings.channel_settings.shm_settings.shm_size = size;
  settings.period = base::TimeDelta::FromMilliseconds(30);

  test->RequestPublish(channel_type, settings);
  test->RequestSubscribe(channel_type, settings,
                         base::BindRepeating(&MessageChecker::CheckMessage,
                                             base::Unretained(checker)));
  test->NotifySubscriber();
}

void PublishMessage(PubSubTest* test, const SimpleMessage& message) {
  test->Publish(message);
}

void PublishAndSubscribeTopic(PubSubTest* test, int channel_type) {
  MessageChecker checker;
  checker.set_test_num(1);
  checker.set_on_test_done(
      base::BindOnce(&PubSubTest::Release, base::Unretained(test)));
  MainThread& main_thread = MainThread::GetInstance();
  main_thread.PostTask(FROM_HERE,
                       base::BindOnce(&SetupPubSub, test,
                                      ChannelDef::CHANNEL_TYPE_TCP, &checker));
  SimpleMessage message = test->GenerateMessage();
  checker.set_expected(message);
  main_thread.PostDelayedTask(FROM_HERE,
                              base::BindOnce(&PublishMessage, test, message),
                              base::TimeDelta::FromMilliseconds(100));

  // Wait for publisher / subscriber to change expected state.
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(500));
  checker.ExpectTestCompleted();
}

TEST_F(PubSubTest, PublishAndSubscribeTopic) {
  PublishAndSubscribeTopic(this, ChannelDef::CHANNEL_TYPE_TCP);
  PublishAndSubscribeTopic(this, ChannelDef::CHANNEL_TYPE_UDP);
  PublishAndSubscribeTopic(this, ChannelDef::CHANNEL_TYPE_SHM);
}

}  // namespace felicia