#include "felicia/core/message/protobuf_util.h"

#include "google/protobuf/reflection.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {
namespace protobuf {

constexpr size_t kMaximumContentLength = 100;

void ProtobufMessageToString(const ::google::protobuf::Message& message,
                             int depth, std::string* out);

void ProtobufMessageToString(
    std::vector<std::string>& entities, int depth,
    const ::google::protobuf::Reflection* reflection,
    const ::google::protobuf::Message& message,
    const ::google::protobuf::FieldDescriptor* field_desc) {
  switch (field_desc->type()) {
    case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<double>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetDouble(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<float>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetFloat(message, field_desc)));
      return;
    case ::google::protobuf::FieldDescriptor::TYPE_INT64:
    case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
    case ::google::protobuf::FieldDescriptor::TYPE_SINT64: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int64_t>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetInt64(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
    case ::google::protobuf::FieldDescriptor::TYPE_FIXED64: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<uint64_t>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetUInt64(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_INT32:
    case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
    case ::google::protobuf::FieldDescriptor::TYPE_SINT32: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int32_t>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetInt32(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
    case ::google::protobuf::FieldDescriptor::TYPE_UINT32: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<uint32_t>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          ::base::NumberToString(reflection->GetUInt32(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_BOOL: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<bool>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(
              strings::BoolToString(reflection->GetBool(message, field_desc)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      entities.push_back(
          strings::BoolToString(reflection->GetBool(message, field_desc)));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_STRING: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<std::string>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(repeated_field_ref.Get(i));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      std::string scratch;
      entities.push_back(
          reflection->GetStringReference(message, field_desc, &scratch));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
      std::string str;
      ProtobufMessageToString(reflection->GetMessage(message, field_desc),
                              depth + 1, &str);
      entities.push_back(::base::StrCat({"{\n", str, "}"}));
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_BYTES: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<std::string>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          std::string data = repeated_field_ref.Get(i);
          if (data.size() > kMaximumContentLength) {
            entities.push_back(::base::StrCat(
                {"[BYTES(", ::base::NumberToString(data.length()), ")]"}));
          } else {
            entities.push_back(data);
          }
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      std::string scratch;
      const std::string& data =
          reflection->GetStringReference(message, field_desc, &scratch);
      if (data.size() > kMaximumContentLength) {
        entities.push_back(::base::StrCat(
            {"[BYTES(", ::base::NumberToString(data.length()), ")]"}));
      } else {
        entities.push_back(data);
      }
      return;
    }
    case ::google::protobuf::FieldDescriptor::TYPE_ENUM: {
      if (field_desc->is_repeated()) {
        auto repeated_field_ref =
            reflection->GetRepeatedFieldRef<int>(message, field_desc);
        entities.push_back("[ ");
        for (int i = 0; i < repeated_field_ref.size(); ++i) {
          entities.push_back(::base::NumberToString(repeated_field_ref.Get(i)));
          if (i != repeated_field_ref.size() - 1) {
            entities.push_back(", ");
          }
        }
        entities.push_back(" ]");
        return;
      }
      const ::google::protobuf::EnumValueDescriptor* enum_value_desc =
          reflection->GetEnum(message, field_desc);
      entities.push_back(::base::NumberToString(enum_value_desc->number()));
      return;
    }
    default:
      LOG(ERROR) << "Not supported type: " << field_desc->type_name();
      return;
  }
}

void ProtobufMessageToString(const ::google::protobuf::Message& message,
                             int depth, std::string* out) {
  const ::google::protobuf::Descriptor* descriptor = message.GetDescriptor();
  const ::google::protobuf::Reflection* reflection = message.GetReflection();

  std::vector<std::string> entites;
  entites.reserve((descriptor->field_count() + depth) * 3);

  for (int i = 0; i < descriptor->field_count(); ++i) {
    const ::google::protobuf::FieldDescriptor* field_desc =
        descriptor->field(i);
    for (int j = 0; j < depth; j++) entites.push_back("  ");
    entites.push_back(
        TextStyle::Blue(::base::StrCat({field_desc->name(), ": "})));
    ProtobufMessageToString(entites, depth, reflection, message, field_desc);
    entites.push_back("\n");
  }

  ::base::StrAppend(out, entites);
}

std::string ProtobufMessageToString(
    const ::google::protobuf::Message& message) {
  std::string ret;
  ProtobufMessageToString(message, 0, &ret);
  return ret;
}

}  // namespace protobuf
}  // namespace felicia