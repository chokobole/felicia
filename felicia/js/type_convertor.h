#ifndef FELICIA_JS_TYPE_CONVERTOR_H_
#define FELICIA_JS_TYPE_CONVERTOR_H_

#include <type_traits>

#include "felicia/js/type_convertor_forward.h"

namespace felicia {
namespace js {

template <>
class TypeConvertor<bool> {
 public:
  static bool ToNativeValue(Napi::Value value) {
    return value.As<Napi::Boolean>().Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, bool value) {
    return Napi::Boolean::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_integral<T>::value &&
                                        std::is_signed<T>::value &&
                                        sizeof(T) <= sizeof(int32_t)>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    return value.As<Napi::Number>().Int32Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<
    T, std::enable_if_t<
           std::is_integral<T>::value && !std::is_same<bool, T>::value &&
           !std::is_signed<T>::value && sizeof(T) <= sizeof(int32_t)>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    return value.As<Napi::Number>().Uint32Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_integral<T>::value &&
                                        !std::is_same<bool, T>::value &&
                                        (sizeof(T) > sizeof(int32_t))>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    // Should return with Napi::BigInt once released officialy.
    return value.As<Napi::Number>().Int64Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<float, T>::value>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    return value.As<Napi::Number>().FloatValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<double, T>::value>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    return value.As<Napi::Number>().DoubleValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<std::string, T>::value>> {
 public:
  static T ToNativeValue(Napi::Value value) {
    return value.As<Napi::String>().Utf8Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, const std::string& value) {
    return Napi::String::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<const char*, T>::value>> {
 public:
  static const char* ToNativeValue(Napi::Value value) {
    LOG(WARNING) << "This pointer may be released before using..";
    return value.As<Napi::String>().Utf8Value().c_str();
  }

  static Napi::Value ToJSValue(Napi::Env env, const char* value) {
    return Napi::String::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_enum<T>::value>> {
 public:
  static std::underlying_type_t<T> ToNativeValue(Napi::Value value) {
    return TypeConvertor<std::underlying_type_t<T>>::ToNativeValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env,
                             static_cast<std::underlying_type_t<T>>(value));
  }
};

}  // namespace js
}  // namespace felicia

#endif  // FELICIA_JS_TYPE_CONVERTOR_H_