#ifndef FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_
#define FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_

#include <memory>

#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/message.h"
#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/no_destructor.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class FEL_EXPORT ProtobufLoader {
 public:
  class FEL_EXPORT ErrorCollector
      : public google::protobuf::DescriptorPool::ErrorCollector {
    void AddError(
        const std::string& filename, const std::string& element_name,
        const google::protobuf::Message* descriptor,
        google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation
            location,
        const std::string& message) override;

    void AddWarning(
        const std::string& filename, const std::string& element_name,
        const google::protobuf::Message* descriptor,
        google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation
            location,
        const std::string& message) override;
  };

  ~ProtobufLoader();

  static std::unique_ptr<ProtobufLoader> Load(const base::FilePath& path);

  const google::protobuf::Message* NewMessage(const std::string& type_name);

 private:
  ProtobufLoader(google::protobuf::compiler::DiskSourceTree* source_tree);

  google::protobuf::compiler::SourceTreeDescriptorDatabase
      source_tree_database_;
  google::protobuf::DescriptorPool descriptor_pool_;
  google::protobuf::DynamicMessageFactory message_factory_;

  ErrorCollector error_collector_;

  DISALLOW_COPY_AND_ASSIGN(ProtobufLoader);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_PROTOBUF_LOADER_H_