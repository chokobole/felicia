#ifndef FELICIA_CORE_MESSAGE_MESSAGE_FILTER_H_
#define FELICIA_CORE_MESSAGE_MESSAGE_FILTER_H_

#include "gtest/gtest_prod.h"

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/sequence_checker.h"
#include "third_party/chromium/base/stl_util.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/template_util.h"
#include "felicia/core/lib/containers/pool.h"

namespace felicia {
namespace internal {

template <uint8_t Idx, typename... Rest>
class MessageQueueImpl;

template <uint8_t Idx, typename MessageTy>
class MessageQueueImpl<Idx, MessageTy> {
 public:
  typedef Pool<MessageTy, uint8_t> PoolType;
  explicit MessageQueueImpl(uint8_t capacity = 1) : pool_(capacity) {}

  void reserve(uint8_t capacity) { pool_.reserve(capacity); }

  PoolType& pool() { return pool_; }
  const PoolType& pool() const { return pool_; }

  PoolType& get() { return pool_; }

  const PoolType& get() const { return pool_; }

 private:
  PoolType pool_;
};

template <uint8_t Idx, typename MessageTy, typename... Rest>
class MessageQueueImpl<Idx, MessageTy, Rest...> {
 public:
  typedef Pool<MessageTy, uint8_t> PoolType;
  explicit MessageQueueImpl(uint8_t capacity = 1) : pool_(capacity) {}

  void reserve(uint8_t capacity) {
    pool_.reserve(capacity);
    rest_.reserve(capacity);
  }

  PoolType& pool() { return pool_; }
  const PoolType& pool() const { return pool_; }

  template <uint8_t N, std::enable_if_t<N == Idx>* = nullptr>
  PoolType& get() {
    return pool_;
  }

  template <uint8_t N,
            std::enable_if_t<(N != Idx && sizeof...(Rest) > 1)>* = nullptr>
  auto& get() {
    return rest_.template get<N>();
  }

  template <uint8_t N,
            std::enable_if_t<(N != Idx && sizeof...(Rest) == 1)>* = nullptr>
  auto& get() {
    return rest_.get();
  }

  template <uint8_t N, std::enable_if_t<N == Idx>* = nullptr>
  const PoolType& get() const {
    return pool_;
  }

  template <uint8_t N,
            std::enable_if_t<(N != Idx && sizeof...(Rest) > 1)>* = nullptr>
  const auto& get() const {
    return rest_.template get<N>();
  }

  template <uint8_t N,
            std::enable_if_t<(N != Idx && sizeof...(Rest) == 1)>* = nullptr>
  const auto& get() const {
    return rest_.get();
  }

 private:
  PoolType pool_;
  MessageQueueImpl<Idx + 1, Rest...> rest_;
};

template <uint8_t Idx>
class MessageQueueImpl<Idx> {
 public:
  MessageQueueImpl() = default;
};

}  // namespace internal

template <typename... Types>
class MessageFilter;

template <typename MessageTy, typename... Rest>
class MessageFilter<MessageTy, Rest...> {
 public:
  typedef internal::MessageQueueImpl<0, MessageTy, Rest...>
      MessageQueueImplType;
  template <uint8_t N>
  using Type =
      internal::PickTypeListItem<N,
                                 base::internal::TypeList<MessageTy, Rest...>>;
  using FilterCallback = base::RepeatingCallback<bool(MessageFilter& filter)>;
  using NotifyCallback = base::RepeatingCallback<void(MessageTy&&, Rest&&...)>;
  enum { TypeSize = sizeof...(Rest) + 1 };

  explicit MessageFilter(uint8_t capacity = 1)
      : filter_impl_(capacity), non_empty_queue_count_(0) {
    DETACH_FROM_SEQUENCE(sequence_checker_);
  }

  ~MessageFilter() { DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_); }

  void reserve(uint8_t capacity) { filter_impl_.reserve(capacity); }

  void set_filter_callback(FilterCallback filter_callback) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    filter_callback_ = filter_callback;
  }

  void set_notify_callback(NotifyCallback notify_callback) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    notify_callback_ = notify_callback;
  }

  // NOTE: Please do not call this from FilterCallback.
  template <uint8_t N>
  void OnMessage(Type<N>&& message) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    push<N>(std::move(message));
  }

  template <uint8_t N,
            std::enable_if_t<N<TypeSize>* = nullptr> void DropMessage() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    pop<N>();
  }

  template <uint8_t N, std::enable_if_t<N >= TypeSize>* = nullptr>
  void DropMessage() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    NOTREACHED();
  }

  void DropMessage(uint8_t idx) {
    switch (idx) {
      case 0:
        DropMessage<0>();
        break;
      case 1:
        DropMessage<1>();
        break;
      case 2:
        DropMessage<2>();
        break;
      case 3:
        DropMessage<3>();
        break;
      case 4:
        DropMessage<4>();
        break;
      case 5:
        DropMessage<5>();
        break;
      case 6:
        DropMessage<6>();
        break;
      case 7:
        DropMessage<7>();
        break;
      case 8:
        DropMessage<8>();
        break;
      default:
        NOTIMPLEMENTED();
    }
  }

  template <uint8_t N>
  const auto& PeekMessage(uint8_t idx) const {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    return get<N>()[idx];
  }

  template <uint8_t N, std::enable_if_t<N<TypeSize>* = nullptr> uint8_t
                           MessageCount() const {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    return get<N>().size();
  }

  template <uint8_t N, std::enable_if_t<N >= TypeSize>* = nullptr>
  uint8_t MessageCount() const {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    NOTREACHED();
    return 0;
  }

  uint8_t MessageCount(uint8_t idx) {
    switch (idx) {
      case 0:
        return MessageCount<0>();
        break;
      case 1:
        return MessageCount<1>();
        break;
      case 2:
        return MessageCount<2>();
        break;
      case 3:
        return MessageCount<3>();
        break;
      case 4:
        return MessageCount<4>();
        break;
      case 5:
        return MessageCount<5>();
        break;
      case 6:
        return MessageCount<6>();
        break;
      case 7:
        return MessageCount<7>();
        break;
      case 8:
        return MessageCount<8>();
        break;
      default:
        NOTIMPLEMENTED();
    }
    return 0;
  }

  // Return whether all the queue have element(s).
  bool DoesAllQueueHaveElement() const {
    return non_empty_queue_count_ == TypeSize;
  }

 private:
  template <uint8_t N>
  void push(Type<N>&& message) {
    auto& p = get<N>();
    bool empty = p.empty();
    p.push(std::move(message));
    if (empty) {
      IncrementNonEmptyQueueCount();
    }
    if (DoesAllQueueHaveElement()) ApplyFilter();
  }

  template <uint8_t N>
  void pop() {
    auto& p = get<N>();
    p.pop();
    if (p.empty()) DecrementNonEmptyQueueCount();
  }

  template <uint8_t N>
  auto& get() {
    return filter_impl_.template get<N>();
  }

  template <uint8_t N>
  const auto& get() const {
    return filter_impl_.template get<N>();
  }

  template <uint8_t N, typename... Args, std::enable_if_t<(N == 0)>* = nullptr>
  void Notify(Args&&... args) {
    auto& p = get<N>();
    auto message = std::move(p.front());
    pop<N>();
    notify_callback_.Run(std::move(message), std::forward<Args>(args)...);
  }

  template <uint8_t N, typename... Args, std::enable_if_t<(N != 0)>* = nullptr>
  void Notify(Args&&... args) {
    auto& p = get<N>();
    auto message = std::move(p.front());
    pop<N>();
    Notify<N - 1>(std::move(message), std::forward<Args>(args)...);
  }

  void ApplyFilter() {
    CHECK(!notify_callback_.is_null());
    bool should_notify = false;
    if (!filter_callback_.is_null() && DoesAllQueueHaveElement()) {
      should_notify = filter_callback_.Run(*this);
    }
    if (should_notify || filter_callback_.is_null()) Notify<TypeSize - 1>();
  }

  // Increment |non_empty_queue_count_|, and return whether all the queue have
  // element(s).
  bool IncrementNonEmptyQueueCount() {
    return ++non_empty_queue_count_ == TypeSize;
  }

  // Decrement |non_empty_queue_count_|, and return the current value.
  uint8_t DecrementNonEmptyQueueCount() { return --non_empty_queue_count_; }

  MessageQueueImplType filter_impl_;
  uint8_t non_empty_queue_count_;
  FilterCallback filter_callback_;
  NotifyCallback notify_callback_;

  SEQUENCE_CHECKER(sequence_checker_);
};

template <typename MessageTy, typename... Rest>
class TimeSyncrhonizerMF {
 public:
  TimeSyncrhonizerMF()
      : max_intra_time_difference_(base::TimeDelta::FromMilliseconds(100)),
        min_inter_time_difference_(base::TimeDelta::Min()),
        last_timestamp_(base::TimeDelta::Min()) {}

  void set_max_intra_time_difference(
      base::TimeDelta max_intra_time_difference) {
    max_intra_time_difference_ = max_intra_time_difference;
  }

  void set_min_inter_time_difference(
      base::TimeDelta min_inter_time_difference) {
    min_inter_time_difference_ = min_inter_time_difference;
  }

  bool Callback(MessageFilter<MessageTy, Rest...>& filter) {
    uint8_t peek_idxs[sizeof...(Rest) + 1] = {0};

    bool has_candidate = false;
    uint8_t last_updated_peek_idx = 0;
    base::TimeDelta last_timestamp_candidate;
    while (filter.DoesAllQueueHaveElement()) {
      MessageInfo earliest_message;
      earliest_message.timestamp = base::TimeDelta::Max();
      MessageInfo latest_message;
      latest_message.timestamp = base::TimeDelta::Min();
      FindTimestamps<sizeof...(Rest)>(filter, peek_idxs, &earliest_message,
                                      &latest_message);

      base::TimeDelta delta =
          latest_message.timestamp - earliest_message.timestamp;
      if (delta <= max_intra_time_difference_ &&
          (last_timestamp_.is_min() ||
           earliest_message.timestamp - last_timestamp_ >=
               min_inter_time_difference_)) {
        last_timestamp_candidate = latest_message.timestamp;
        has_candidate = true;
        // Try to find better candidate
        if (delta.is_zero()) break;
        last_updated_peek_idx = earliest_message.idx;
        if (filter.MessageCount(last_updated_peek_idx) >
            peek_idxs[last_updated_peek_idx] + 1) {
          ++peek_idxs[last_updated_peek_idx];
        } else {
          break;
        }
      } else if (has_candidate) {
        // Failed to find better candidate, just use previous one.
        --peek_idxs[last_updated_peek_idx];
        break;
      } else {
        // Try to find better candidate if it can
        if (filter.MessageCount(earliest_message.idx) > 1) {
          filter.DropMessage(earliest_message.idx);
        } else {
          break;
        }
      }
    }

    if (has_candidate) {
      last_timestamp_ = last_timestamp_candidate;
      // Drop all the messages before peek_idx, which are useless.
      for (uint8_t i = 0; i < base::size(peek_idxs); ++i) {
        for (uint8_t j = 0; j < peek_idxs[i]; ++j) {
          filter.DropMessage(i);
        }
      }
      return true;
    }

    return false;
  }

 private:
  FRIEND_TEST(MessageFilterTest, ApplyTimeSynchronizerFilterTest);

  struct MessageInfo {
    uint8_t idx;
    base::TimeDelta timestamp;
  };

  template <uint8_t N>
  void FindTimestamps(MessageFilter<MessageTy, Rest...>& filter,
                      uint8_t* peek_idxs, MessageInfo* earliest_message,
                      MessageInfo* latest_message) {
    base::TimeDelta timestamp = base::TimeDelta::FromMicroseconds(
        filter.template PeekMessage<N>(peek_idxs[N]).timestamp());
    if (timestamp < earliest_message->timestamp) {
      earliest_message->timestamp = timestamp;
      earliest_message->idx = N;
    }
    if (timestamp > latest_message->timestamp) {
      latest_message->timestamp = timestamp;
      latest_message->idx = N;
    }

    if (N == 0) return;
    constexpr uint8_t NEXT = N - 1;
    FindTimestamps<NEXT>(filter, peek_idxs, earliest_message, latest_message);
  }

  // Maximum time difference between the earliest message and the latest mesage
  // in the set of message,
  base::TimeDelta max_intra_time_difference_;
  // Minimum time difference among the set of messages,
  base::TimeDelta min_inter_time_difference_;
  base::TimeDelta last_timestamp_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_MESSAGE_FILTER_H_