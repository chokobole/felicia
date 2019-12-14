// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSGAE_TEST_SIMPLE_MESSAGE_H_
#define FELICIA_CORE_MESSGAE_TEST_SIMPLE_MESSAGE_H_

#include <type_traits>
#include <vector>

namespace felicia {
namespace test {

class SimpleMessage {
 public:
  SimpleMessage() = default;
  SimpleMessage(int data, double timestamp)
      : data_(data), timestamp_(timestamp) {}
  ~SimpleMessage() = default;

  void set_data(int data) { data_ = data; }
  int data() const { return data_; }

  void set_timestamp(double timestamp) { timestamp_ = timestamp; }
  double timestamp() const { return timestamp_; }

  bool operator==(const SimpleMessage& other) const {
    return data_ == other.data_ && timestamp_ == other.timestamp_;
  }
  bool operator!=(const SimpleMessage& other) const {
    return !operator==(other);
  }

 private:
  int data_;
  double timestamp_;
};

void GenerateSimpleMessageLinearly(int data_start, int data_delta,
                                   double timestamp_start,
                                   double timestamp_delta, size_t count,
                                   std::vector<SimpleMessage>* out) {
  out->clear();
  out->reserve(count);
  for (size_t i = 0; i < count; ++i) {
    out->emplace_back(data_start + i * data_delta,
                      timestamp_start + i * timestamp_delta);
  }
}

}  // namespace test
}  // namespace felicia

#endif  // FELICIA_CORE_MESSGAE_TEST_A_MESSAGE_H_