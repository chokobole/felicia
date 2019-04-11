#ifndef FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_
#define FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_

#include <memory>

#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/message.h"
#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/no_destructor.h"

namespace felicia {

class ProtobufLoader {
 public:
  ~ProtobufLoader();

  static std::unique_ptr<ProtobufLoader> Load(const ::base::FilePath& path);

  const ::google::protobuf::Message* NewMessage(const std::string& type_name);

 private:
  ProtobufLoader();

  ::google::protobuf::DescriptorPool descriptor_pool_;

  ::google::protobuf::DynamicMessageFactory message_factory_;

  DISALLOW_COPY_AND_ASSIGN(ProtobufLoader);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_