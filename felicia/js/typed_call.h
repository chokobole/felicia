#ifndef FELICIA_JS_TYPED_CALL_H_
#define FELICIA_JS_TYPED_CALL_H_

#include "felicia/js/type_convertor.h"
#include "napi.h"

//
// You can check how to use from 'test/typed_call_test.cc'
//
namespace felicia {

namespace internal {

template <typename... Types>
struct TypeList {};

template <size_t n, typename List>
struct PickTypeListItemImpl;

template <size_t n, typename T, typename... List>
struct PickTypeListItemImpl<n, TypeList<T, List...>>
    : PickTypeListItemImpl<n - 1, TypeList<List...>> {};

template <typename T, typename... List>
struct PickTypeListItemImpl<0, TypeList<T, List...>> {
  using Type = std::decay_t<T>;
};

template <size_t n, typename List>
using PickTypeListItem = typename PickTypeListItemImpl<n, List>::Type;

template <typename T>
std::enable_if_t<std::is_same<bool, T>::value, ::Napi::Boolean> ToJs(
    ::Napi::Env env, T value) {
  return ::Napi::Boolean::New(env, value);
}

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<bool, T>::value,
                 ::Napi::Number>
ToJs(::Napi::Env env, T value) {
  return ::Napi::Number::New(env, value);
}

template <typename T>
std::enable_if_t<std::is_same<std::string, T>::value, ::Napi::String> ToJs(
    ::Napi::Env env, const T& value) {
  return ::Napi::String::New(env, value);
}

template <typename R, typename... Args>
std::enable_if_t<0 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (*f)(Args...)) {
  return f();
}

template <typename R, typename... Args>
std::enable_if_t<1 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]));
}

template <typename R, typename... Args>
std::enable_if_t<2 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]),
      js::TypeConvertor<internal::PickTypeListItem<1, ArgList>>::ToNativeValue(
          info[1]));
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<0 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  return ((*c).*f)();
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<1 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]));
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<2 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]),
      js::TypeConvertor<internal::PickTypeListItem<1, ArgList>>::ToNativeValue(
          info[1]));
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<0 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
    const Class* c) {
  return ((*c).*f)();
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<1 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
    const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]));
}

template <typename R, typename Class, typename... Args>
std::enable_if_t<2 == sizeof...(Args), R> Invoke(
    const ::Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
    const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(
      js::TypeConvertor<internal::PickTypeListItem<0, ArgList>>::ToNativeValue(
          info[0]),
      js::TypeConvertor<internal::PickTypeListItem<1, ArgList>>::ToNativeValue(
          info[1]));
}

}  // namespace internal

#define THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)            \
  ::Napi::TypeError::New(env, "Wrong number of arguments") \
      .ThrowAsJavaScriptException()

#define JS_CHECK_NUM_ARGS(env, num_args) \
  if (info.Length() != num_args) THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)

template <typename R, typename... Args>
::Napi::Value TypedCall(const ::Napi::CallbackInfo& info, R (*f)(Args...)) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return js::TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f));
}

template <typename... Args>
void TypedCall(const ::Napi::CallbackInfo& info, void (*f)(Args...)) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f);
}

template <typename R, typename Class, typename... Args>
::Napi::Value TypedCall(const ::Napi::CallbackInfo& info,
                        R (Class::*f)(Args...), Class* c) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return js::TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const ::Napi::CallbackInfo& info, void (Class::*f)(Args...),
               Class* c) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

template <typename R, typename Class, typename... Args>
::Napi::Value TypedCall(const ::Napi::CallbackInfo& info,
                        R (Class::*f)(Args...) const, const Class* c) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return js::TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const ::Napi::CallbackInfo& info,
               void (Class::*f)(Args...) const, const Class* c) {
  ::Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

}  // namespace felicia

#endif  // FELICIA_JS_TYPED_CALL_H_