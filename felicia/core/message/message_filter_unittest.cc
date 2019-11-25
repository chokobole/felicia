// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/message/message_filter.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/message/test/a_message.h"

namespace felicia {

namespace {

template <typename T, typename T2>
class NotifyCallbackChecker {
 public:
  NotifyCallbackChecker() : call_count(0) {}

  void OnNotify(T&& a, T2&& b) {
    EXPECT_EQ(std::get<0>(answers[call_count]), a);
    EXPECT_EQ(std::get<1>(answers[call_count]), b);
    call_count++;
  }

  void reset() {
    call_count = 0;
    answers.clear();
  }

  size_t call_count;
  std::vector<std::tuple<T, T2>> answers;
};

template <typename T, typename U>
void PublishInSequence(MessageFilter<AMessage<T>, AMessage<U>>& filter,
                       std::vector<AMessage<T>>& messages,
                       std::vector<AMessage<U>>& messages2) {
  size_t i = 0, j = 0;
  while (i < messages.size() || j < messages2.size()) {
    if (i < messages.size() && j < messages.size()) {
      if (messages[i].timestamp() <= messages2[j].timestamp()) {
        filter.template OnMessage<0>(std::move(messages[i++]));
      } else {
        filter.template OnMessage<1>(std::move(messages2[j++]));
      }
    } else if (i < messages.size()) {
      filter.template OnMessage<0>(std::move(messages[i++]));
    } else {
      filter.template OnMessage<1>(std::move(messages2[j++]));
    }
  }
}

}  // namespace

TEST(MessageFilterTest, BasicTest) {
  NotifyCallbackChecker<int, double> checker;
  std::vector<std::tuple<int, double>> inputs = {
      std::tuple<int, double>{3, 3.5}, std::tuple<int, double>{5, 6.7}};
  checker.answers = inputs;
  MessageFilter<int, double> filter;
  filter.set_notify_callback(
      base::BindRepeating(&NotifyCallbackChecker<int, double>::OnNotify,
                          base::Unretained(&checker)));
  filter.OnMessage<0>(std::move(std::get<0>(inputs[0])));
  filter.OnMessage<1>(std::move(std::get<1>(inputs[0])));
  filter.OnMessage<0>(std::move(std::get<0>(inputs[1])));
  filter.OnMessage<1>(std::move(std::get<1>(inputs[1])));
  EXPECT_EQ(checker.answers.size(), checker.call_count);
}

TEST(MessageFilterTest, ApplyBasicFilterTest) {
  NotifyCallbackChecker<int, double> checker;
  std::vector<std::tuple<int, double>> inputs = {
      std::tuple<int, double>{3, 3.5}, std::tuple<int, double>{5, 6.7}};
  checker.answers = {std::tuple<int, double>{3, 6.7}};
  MessageFilter<int, double> filter(10);
  filter.set_filter_callback(
      base::BindRepeating([](MessageFilter<int, double>& filter) {
        int i = filter.PeekMessage<0>(0);
        double d = filter.PeekMessage<1>(0);
        if (i == 3 && d == 6.7) return true;
        if (i != 3) filter.DropMessage<0>();
        if (d != 6.7) filter.DropMessage<1>();
        return false;
      }));
  filter.set_notify_callback(
      base::BindRepeating(&NotifyCallbackChecker<int, double>::OnNotify,
                          base::Unretained(&checker)));
  filter.OnMessage<0>(std::move(std::get<0>(inputs[0])));
  filter.OnMessage<1>(std::move(std::get<1>(inputs[0])));
  filter.OnMessage<0>(std::move(std::get<0>(inputs[1])));
  filter.OnMessage<1>(std::move(std::get<1>(inputs[1])));
  EXPECT_EQ(checker.answers.size(), checker.call_count);
}

TEST(MessageFilterTest, ApplyTimeSynchronizerFilterTest) {
  NotifyCallbackChecker<IntMessage, IntMessage> checker;
  std::vector<IntMessage> messages;
  std::vector<IntMessage> messages2;
  GenerateAMessageLinearly(0, 1, 0, 200000, 8, &messages);
  GenerateAMessageLinearly(0, 1, 0, 300000, 8, &messages2);

  // CASE 1: when queue size is 1
  {
    checker.answers = {
        std::tuple<IntMessage, IntMessage>{{0, 0}, {0, 0}},
        std::tuple<IntMessage, IntMessage>{{1, 200000}, {1, 300000}},
        std::tuple<IntMessage, IntMessage>{{3, 600000}, {2, 600000}},
        std::tuple<IntMessage, IntMessage>{{4, 800000}, {3, 900000}},
        std::tuple<IntMessage, IntMessage>{{6, 1200000}, {4, 1200000}},
        std::tuple<IntMessage, IntMessage>{{7, 1400000}, {5, 1500000}},
    };

    MessageFilter<IntMessage, IntMessage> filter;
    TimeSyncrhonizerMF<IntMessage, IntMessage> synchronizer;
    filter.set_filter_callback(base::BindRepeating(
        &TimeSyncrhonizerMF<IntMessage, IntMessage>::Callback,
        base::Unretained(&synchronizer)));
    filter.set_notify_callback(base::BindRepeating(
        &NotifyCallbackChecker<IntMessage, IntMessage>::OnNotify,
        base::Unretained(&checker)));
    PublishInSequence(filter, messages, messages2);
    EXPECT_EQ(checker.answers.size(), checker.call_count);
  }

  // CASE 2: when queue size is 10
  {
    checker.reset();
    checker.answers = {
        std::tuple<IntMessage, IntMessage>{{0, 0}, {0, 0}},
        std::tuple<IntMessage, IntMessage>{{1, 200000}, {1, 300000}},
        std::tuple<IntMessage, IntMessage>{{3, 600000}, {2, 600000}},
        std::tuple<IntMessage, IntMessage>{{4, 800000}, {3, 900000}},
        std::tuple<IntMessage, IntMessage>{{6, 1200000}, {4, 1200000}},
        std::tuple<IntMessage, IntMessage>{{7, 1400000}, {5, 1500000}},
    };

    MessageFilter<IntMessage, IntMessage> filter(10);
    TimeSyncrhonizerMF<IntMessage, IntMessage> synchronizer;
    filter.set_filter_callback(base::BindRepeating(
        &TimeSyncrhonizerMF<IntMessage, IntMessage>::Callback,
        base::Unretained(&synchronizer)));
    filter.set_notify_callback(base::BindRepeating(
        &NotifyCallbackChecker<IntMessage, IntMessage>::OnNotify,
        base::Unretained(&checker)));
    PublishInSequence(filter, messages, messages2);
    EXPECT_EQ(checker.answers.size(), checker.call_count);
  }

  // CASE 3: when max_intra_time_difference is 0
  {
    checker.reset();
    checker.answers = {
        std::tuple<IntMessage, IntMessage>{{0, 0}, {0, 0}},
        std::tuple<IntMessage, IntMessage>{{3, 600000}, {2, 600000}},
        std::tuple<IntMessage, IntMessage>{{6, 1200000}, {4, 1200000}},
    };

    MessageFilter<IntMessage, IntMessage> filter(10);
    TimeSyncrhonizerMF<IntMessage, IntMessage> synchronizer;
    synchronizer.set_max_intra_time_difference(base::TimeDelta());
    filter.set_filter_callback(base::BindRepeating(
        &TimeSyncrhonizerMF<IntMessage, IntMessage>::Callback,
        base::Unretained(&synchronizer)));
    filter.set_notify_callback(base::BindRepeating(
        &NotifyCallbackChecker<IntMessage, IntMessage>::OnNotify,
        base::Unretained(&checker)));
    PublishInSequence(filter, messages, messages2);
    EXPECT_EQ(checker.answers.size(), checker.call_count);
  }

  // CASE 4: when min_inter_time_difference is 300ms
  {
    checker.reset();
    checker.answers = {
        std::tuple<IntMessage, IntMessage>{{0, 0}, {0, 0}},
        std::tuple<IntMessage, IntMessage>{{2, 400000}, {1, 300000}},
        std::tuple<IntMessage, IntMessage>{{4, 800000}, {3, 900000}},
        std::tuple<IntMessage, IntMessage>{{6, 1200000}, {4, 1200000}},
    };

    MessageFilter<IntMessage, IntMessage> filter(10);
    TimeSyncrhonizerMF<IntMessage, IntMessage> synchronizer;
    synchronizer.set_min_inter_time_difference(
        base::TimeDelta::FromMilliseconds(300));
    filter.set_filter_callback(base::BindRepeating(
        &TimeSyncrhonizerMF<IntMessage, IntMessage>::Callback,
        base::Unretained(&synchronizer)));
    filter.set_notify_callback(base::BindRepeating(
        &NotifyCallbackChecker<IntMessage, IntMessage>::OnNotify,
        base::Unretained(&checker)));
    PublishInSequence(filter, messages, messages2);
    EXPECT_EQ(checker.answers.size(), checker.call_count);
  }
}

}  // namespace felicia