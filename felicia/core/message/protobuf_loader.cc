#include "felicia/core/message/protobuf_loader.h"

#include "third_party/chromium/base/files/file_enumerator.h"
#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

ProtobufLoader::ProtobufLoader() {}

ProtobufLoader::~ProtobufLoader() {}

// static
std::unique_ptr<ProtobufLoader> ProtobufLoader::Load(
    const ::base::FilePath& root_path) {
  ::base::FilePath new_root_path = root_path.AsEndingWithSeparator();

  ::google::protobuf::compiler::DiskSourceTree source_tree;
  source_tree.MapPath("", new_root_path.value());
  ::google::protobuf::io::ZeroCopyInputStream* input_stream =
      source_tree.Open("");
  if (!input_stream) {
    LOG(ERROR) << "Failed to load" << source_tree.GetLastErrorMessage();
    return nullptr;
  }

  ::google::protobuf::compiler::SourceTreeDescriptorDatabase database(
      &source_tree);

  ProtobufLoader* loader = new ProtobufLoader();
  ::base::FileEnumerator enumerator(
      new_root_path, true, ::base::FileEnumerator::FILES,
      FILE_PATH_LITERAL("*.proto"),
      ::base::FileEnumerator::FolderSearchPolicy::ALL);

  std::vector<std::string> failed_file_paths;
  for (auto path = enumerator.Next(); !path.empty(); path = enumerator.Next()) {
    auto relative_path = path.value().substr(new_root_path.value().length());
    if (!strings::StartsWith(relative_path, "felicia")) continue;

    ::google::protobuf::FileDescriptorProto proto;
    if (!database.FindFileByName(relative_path, &proto)) {
      LOG(ERROR) << "Failed to FindFileByName " << path;
      continue;
    }
    const ::google::protobuf::FileDescriptor* descriptor =
        loader->descriptor_pool_.BuildFile(proto);
    if (!descriptor) {
      failed_file_paths.push_back(relative_path);
    }
  }

  size_t last_size = 0;
  while (last_size != failed_file_paths.size()) {
    last_size = failed_file_paths.size();
    auto it = failed_file_paths.begin();
    while (it != failed_file_paths.end()) {
      ::google::protobuf::FileDescriptorProto proto;
      database.FindFileByName(*it, &proto);

      const ::google::protobuf::FileDescriptor* descriptor =
          loader->descriptor_pool_.BuildFile(proto);
      if (descriptor) {
        it = failed_file_paths.erase(it);
        continue;
      }
      it++;
    }
  }

  for (auto& failed_path : failed_file_paths) {
    LOG(ERROR) << "Failed to Parse. path : " << failed_path;
  }

  return ::base::WrapUnique(std::move(loader));
}

const ::google::protobuf::Message* ProtobufLoader::NewMessage(
    const std::string& type_name) {
  const ::google::protobuf::Descriptor* descriptor =
      descriptor_pool_.FindMessageTypeByName(type_name);
  if (!descriptor) {
    LOG(ERROR) << "Failed to find message type: " << type_name;
    return nullptr;
  }

  return message_factory_.GetPrototype(descriptor);
}

}  // namespace felicia