// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSGAE_TEST_A_MESSAGE_H_
#define FELICIA_CORE_MESSGAE_TEST_A_MESSAGE_H_

#include <type_traits>
#include <vector>

namespace felicia {

template <typename T>
class AMessage {
 public:
  AMessage() = default;
  AMessage(const T& data, double timestamp)
      : data_(data), timestamp_(timestamp) {}
  ~AMessage() = default;

  void set_data(const T& data) { data_ = data; }
  const T& data() const { return data_; }

  void set_timestamp(double timestamp) { timestamp_ = timestamp; }
  double timestamp() const { return timestamp_; }

  bool operator==(const AMessage& other) const {
    return data_ == other.data_ && timestamp_ == other.timestamp_;
  }
  bool operator!=(const AMessage& other) const { return !operator==(other); }

 private:
  int data_;
  double timestamp_;
};

typedef AMessage<int> IntMessage;

template <typename T, std::enable_if_t<internal::SupportsPlusOperator<T>::value,
                                       void*> = nullptr>
void GenerateAMessageLinearly(T data_start, T data_delta,
                              double timestamp_start, double timestamp_delta,
                              size_t count, std::vector<AMessage<T>>* out) {
  out->clear();
  out->reserve(count);
  for (size_t i = 0; i < count; ++i) {
    out->emplace_back(data_start + i * data_delta,
                      timestamp_start + i * timestamp_delta);
  }
}

}  // namespace felicia

#endif  // FELICIA_CORE_MESSGAE_TEST_A_MESSAGE_H_