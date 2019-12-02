// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_NODE_BINDING)

#include "napi.h"

#include <iostream>

#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/js/type_conversion/typed_call.h"

namespace felicia {

class TestImpl {
 public:
  void void_return() { std::cout << "void_return" << std::endl; }

  bool bool_return(bool v) {
    std::cout << "bool_return" << std::endl;
    return !v;
  };

  short short_return(short v) {
    std::cout << "short_return" << std::endl;
    return v * 2;
  }

  int int_return(int v) {
    std::cout << "int_return" << std::endl;
    return v * 3;
  }

  float float_return(float v) {
    std::cout << "float_return" << std::endl;
    return v * 4;
  }

  double double_return(double v) {
    std::cout << "double_return" << std::endl;
    return v * 5;
  }

  const char* const_char_ptr_return(const char* v) {
    std::cout << "const_char_ptr_return" << std::endl;
    return "hello";
  }

  std::string string_return(const std::string& v) {
    std::cout << "string_return" << std::endl;
    return base::StrCat({v, " world"});
  }

  const std::string& string_ref_return(const std::string& v) {
    std::cout << "string_ref_return" << std::endl;
    return text;
  }

  static void s_void_return() { std::cout << "s_void_return" << std::endl; }

  static int s_int_return(int v) {
    std::cout << "s_int_return" << std::endl;
    return v * 3;
  }

  static double s_double_return(double v) {
    std::cout << "s_double_return" << std::endl;
    return v * 5;
  }

  static std::string s_string_return(const std::string& v) {
    std::cout << "s_string_return" << std::endl;
    return base::StrCat({v, " world"});
  }

 private:
  std::string text = "thank you";
};

class Test : public Napi::ObjectWrap<Test> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  Test(const Napi::CallbackInfo& info);

  void void_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::void_return, &test_impl_);
  }

  Napi::Value bool_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::bool_return, &test_impl_);
  }

  Napi::Value short_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::short_return, &test_impl_);
  }

  Napi::Value int_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::int_return, &test_impl_);
  }

  Napi::Value float_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::float_return, &test_impl_);
  }

  Napi::Value double_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::double_return, &test_impl_);
  }

  Napi::Value const_char_ptr_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::const_char_ptr_return, &test_impl_);
  }

  Napi::Value string_ref_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::string_ref_return, &test_impl_);
  }

  Napi::Value string_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::string_return, &test_impl_);
  }

  static void s_void_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::s_void_return);
  }

  static Napi::Value s_int_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::s_int_return);
  }

  static Napi::Value s_double_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::s_double_return);
  }

  static Napi::Value s_string_return(const Napi::CallbackInfo& info) {
    return TypedCall(info, &TestImpl::s_string_return);
  }

 private:
  static Napi::FunctionReference constructor_;

  TestImpl test_impl_;
};

Napi::FunctionReference Test::constructor_;

// static
void Test::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(
      env, "Test",
      {
          InstanceMethod("void_return", &Test::void_return),
          InstanceMethod("bool_return", &Test::bool_return),
          InstanceMethod("short_return", &Test::short_return),
          InstanceMethod("int_return", &Test::int_return),
          InstanceMethod("float_return", &Test::float_return),
          InstanceMethod("double_return", &Test::double_return),
          InstanceMethod("const_char_ptr_return", &Test::const_char_ptr_return),
          InstanceMethod("string_return", &Test::string_return),
          InstanceMethod("string_ref_return", &Test::string_ref_return),
          StaticMethod("s_void_return", &Test::s_void_return),
          StaticMethod("s_int_return", &Test::s_int_return),
          StaticMethod("s_double_return", &Test::s_double_return),
          StaticMethod("s_string_return", &Test::s_string_return),

      });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Test", func);
}

Test::Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  test_impl_ = TestImpl();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  Test::Init(env, exports);

  return exports;
}

NODE_API_MODULE(typed_call, Init)

}  // namespace felicia

#endif  // defined(FEL_NODE_BINDING)