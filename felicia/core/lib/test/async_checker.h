#ifndef FELICIA_CORE_LIB_TEST_ASYNC_CHECKER_H_
#define FELICIA_CORE_LIB_TEST_ASYNC_CHECKER_H_

#include "gtest/gtest.h"
#include "third_party/chromium/base/callback.h"

namespace felicia {

class AsyncChecker {
 public:
  void set_test_num(int test_num) { test_num_ = test_num; }

  void set_on_test_done(base::OnceClosure on_test_done) {
    on_test_done_ = std::move(on_test_done);
  }

  void CountDownTest() {
    test_num_--;
    if (test_num_ == 0) {
      if (!on_test_done_.is_null()) std::move(on_test_done_).Run();
    }
  }

  void ExpectTestCompleted() { EXPECT_EQ(test_num_, 0); }

  void AssertTestCompleted() { ASSERT_EQ(test_num_, 0); }

 protected:
  int test_num_ = -1;
  base::OnceClosure on_test_done_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_TEST_ASYNC_CHECKER_H_