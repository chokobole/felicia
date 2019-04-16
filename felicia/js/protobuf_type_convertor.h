#ifndef FELICIA_JS_PROTOBUF_TYPE_CONVERTOR_H_
#define FELICIA_JS_PROTOBUF_TYPE_CONVERTOR_H_

#include "google/protobuf/message.h"

#include "felicia/js/type_convertor_forward.h"

namespace felicia {
namespace js {
template <>
class TypeConvertor<::google::protobuf::Message> {
 public:
  static void ToNativeValue(::Napi::Value value);

  static ::Napi::Value ToJSValue(::Napi::Env env,
                                 const ::google::protobuf::Message& value);

 private:
  static ::Napi::Object ToJSObject(::Napi::Env env,
                                   const ::google::protobuf::Message& value);

  static ::Napi::Value ToJSValue(
      ::Napi::Env env, const ::google::protobuf::Reflection* reflection,
      const ::google::protobuf::Message& message,
      const ::google::protobuf::FieldDescriptor* field_desc);
};

}  // namespace js
}  // namespace felicia

#endif  // FELICIA_JS_PROTOBUF_TYPE_CONVERTOR_H_