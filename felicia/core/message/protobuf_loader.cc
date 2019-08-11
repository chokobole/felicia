#include "felicia/core/message/protobuf_loader.h"

#include "third_party/chromium/base/files/file_enumerator.h"
#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

ProtobufLoader::ProtobufLoader(
    google::protobuf::compiler::DiskSourceTree* source_tree)
    : source_tree_database_(source_tree),
      descriptor_pool_(&source_tree_database_, &error_collector_) {}

ProtobufLoader::~ProtobufLoader() {}

// static
std::unique_ptr<ProtobufLoader> ProtobufLoader::Load(
    const base::FilePath& root_path) {
  base::FilePath root_path_with_separator = root_path.AsEndingWithSeparator();
  std::string root_path_canonicalized;
  base::ReplaceChars(root_path_with_separator.MaybeAsASCII(), "\\", "/",
                     &root_path_canonicalized);

  google::protobuf::compiler::DiskSourceTree source_tree;
  source_tree.MapPath("", root_path_canonicalized);

  ProtobufLoader* loader = new ProtobufLoader(&source_tree);
  base::FileEnumerator enumerator(
      root_path_with_separator, true, base::FileEnumerator::FILES,
      FILE_PATH_LITERAL("*.proto"),
      base::FileEnumerator::FolderSearchPolicy::ALL);

  for (auto path = enumerator.Next(); !path.empty(); path = enumerator.Next()) {
    std::string path_canonicalized;
    base::ReplaceChars(path.MaybeAsASCII(), "\\", "/", &path_canonicalized);
    auto relative_path =
        path_canonicalized.substr(root_path_canonicalized.length());
    if (!StartsWith(relative_path, "felicia/")) continue;

    loader->descriptor_pool_.FindFileByName(relative_path);
  }

  return base::WrapUnique(std::move(loader));
}

const google::protobuf::Message* ProtobufLoader::NewMessage(
    const std::string& type_name) {
  const google::protobuf::Descriptor* descriptor =
      descriptor_pool_.FindMessageTypeByName(type_name);
  if (!descriptor) {
    LOG(ERROR) << "Failed to find message type: " << type_name;
    return nullptr;
  }

  return message_factory_.GetPrototype(descriptor);
}

void ProtobufLoader::ErrorCollector::AddError(
    const std::string& filename, const std::string& element_name,
    const google::protobuf::Message* descriptor,
    google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation location,
    const std::string& message) {
  LOG(ERROR) << message;
}

void ProtobufLoader::ErrorCollector::AddWarning(
    const std::string& filename, const std::string& element_name,
    const google::protobuf::Message* descriptor,
    google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation location,
    const std::string& message) {
  LOG(WARNING) << message;
}

}  // namespace felicia