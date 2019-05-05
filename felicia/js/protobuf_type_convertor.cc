#include "felicia/js/protobuf_type_convertor.h"

#include "google/protobuf/reflection.h"
#include "third_party/chromium/base/logging.h"

namespace felicia {
namespace js {

// static
void TypeConvertor<::google::protobuf::Message>::ToNativeValue(
    ::Napi::Value value) {
  // Is there any chance to use this?
  NOTIMPLEMENTED();
}

// static
::Napi::Value TypeConvertor<::google::protobuf::Message>::ToJSValue(
    ::Napi::Env env, const ::google::protobuf::Message& value) {
  ::Napi::Object obj = ::Napi::Object::New(env);
  obj["type"] = ::Napi::String::New(env, value.GetTypeName());
  obj["message"] = ToJSObject(env, value);

  return obj;
}

// static
::Napi::Object TypeConvertor<::google::protobuf::Message>::ToJSObject(
    ::Napi::Env env, const ::google::protobuf::Message& value) {
  const ::google::protobuf::Descriptor* descriptor = value.GetDescriptor();
  const ::google::protobuf::Reflection* reflection = value.GetReflection();

  ::Napi::Object obj = ::Napi::Object::New(env);
  for (int i = 0; i < descriptor->field_count(); ++i) {
    const ::google::protobuf::FieldDescriptor* field_desc =
        descriptor->field(i);
    obj[field_desc->json_name()] =
        ToJSValue(env, reflection, value, field_desc);
  }

  return obj;
}

// static
::Napi::Value TypeConvertor<::google::protobuf::Message>::ToJSValue(
    ::Napi::Env env, const ::google::protobuf::Reflection* reflection,
    const ::google::protobuf::Message& message,
    const ::google::protobuf::FieldDescriptor* field_desc) {
  switch (field_desc->type()) {
    case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<double>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetDouble(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<float>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetFloat(message, field_desc));
    case ::google::protobuf::FieldDescriptor::TYPE_INT64:
    case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
    case ::google::protobuf::FieldDescriptor::TYPE_SINT64: {
      // Should return with ::Napi::BigInt once released officialy.
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int64_t>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetInt64(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
    case ::google::protobuf::FieldDescriptor::TYPE_FIXED64: {
      // Should return with ::Napi::BigInt once released officialy.
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<uint64_t>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetUInt64(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_INT32:
    case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
    case ::google::protobuf::FieldDescriptor::TYPE_SINT32: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int32_t>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetInt32(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
    case ::google::protobuf::FieldDescriptor::TYPE_UINT32: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<uint32_t>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Number::New(env,
                                 reflection->GetUInt32(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_BOOL: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<bool>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Boolean::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      return ::Napi::Boolean::New(env,
                                  reflection->GetBool(message, field_desc));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_STRING: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<std::string>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::String::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      std::string scratch;
      return ::Napi::String::New(
          env, reflection->GetStringReference(message, field_desc, &scratch));
    }
    case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      return ToJSObject(env, reflection->GetMessage(message, field_desc));
    case ::google::protobuf::FieldDescriptor::TYPE_BYTES: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<std::string>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          std::string data = repeated_field_ref.Get(i);
          ::Napi::ArrayBuffer arrayBuffer = ::Napi::ArrayBuffer::New(
              env, const_cast<char*>(data.data()), data.length());
          array[i] =
              ::Napi::Uint8Array::New(env, data.length(), arrayBuffer, 0);
        }
        return array;
      }
      std::string scratch;
      const std::string& data =
          reflection->GetStringReference(message, field_desc, &scratch);
      ::Napi::ArrayBuffer arrayBuffer = ::Napi::ArrayBuffer::New(
          env, const_cast<char*>(data.data()), data.length());
      return ::Napi::Uint8Array::New(env, data.length(), arrayBuffer, 0);
    }
    case ::google::protobuf::FieldDescriptor::TYPE_ENUM: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int>(message, field_desc);
        ::Napi::Array array =
            ::Napi::Array::New(env, repeated_field_ref.size());
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          array[i] = ::Napi::Number::New(env, repeated_field_ref.Get(i));
        }
        return array;
      }
      const ::google::protobuf::EnumValueDescriptor* enum_value_desc =
          reflection->GetEnum(message, field_desc);
      return ::Napi::Number::New(env, enum_value_desc->number());
    }
    default:
      LOG(ERROR) << "Not supported type: " << field_desc->type_name();
      return env.Undefined();
  }
}

}  // namespace js
}  // namespace felicia