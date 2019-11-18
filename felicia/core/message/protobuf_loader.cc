// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/message/protobuf_loader.h"

#include "third_party/chromium/base/files/file_enumerator.h"
#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/string_tokenizer.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/felicia_env.h"
#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

namespace {

std::vector<base::FilePath> GetProtobufRootPath() {
  std::vector<base::FilePath> root_paths;
#if defined(OS_WIN)
  const wchar_t* protobuf_root_path_str = _wgetenv(L"FEL_PROTOBUF_ROOT_PATH");
#else
  const char* protobuf_root_path_str = getenv("FEL_PROTOBUF_ROOT_PATH");
#endif
  if (protobuf_root_path_str) {
#if defined(OS_WIN)
    std::wstring protobuf_root_path(protobuf_root_path_str);
    base::WStringTokenizer tokens(protobuf_root_path, L";");
#else
    std::string protobuf_root_path(protobuf_root_path_str);
    base::StringTokenizer tokens(protobuf_root_path, ";");
#endif
    while (tokens.GetNext()) {
      base::FilePath path(tokens.token());
      root_paths.push_back(path.AsEndingWithSeparator());
    }
  }
  return root_paths;
}

}  // namespace

ProtobufLoader::ProtobufLoader() = default;

ProtobufLoader::~ProtobufLoader() = default;

// static
ProtobufLoader& ProtobufLoader::GetInstance() {
  static base::NoDestructor<ProtobufLoader> protobuf_loader;
  return *protobuf_loader;
}

void ProtobufLoader::Load() {
  if (source_tree_database_) return;
  std::vector<base::FilePath> root_paths = GetProtobufRootPath();
  google::protobuf::compiler::DiskSourceTree source_tree;
  source_tree_database_.reset(
      new google::protobuf::compiler::SourceTreeDescriptorDatabase(
          &source_tree));
  descriptor_pool_.reset(new google::protobuf::DescriptorPool(
      source_tree_database_.get(), &error_collector_));

#if defined(BAZEL_BUILD)
  {
    base::FilePath root_path(FILE_PATH_LITERAL("") FELICIA_ROOT);
    root_path = root_path.AsEndingWithSeparator();
    root_paths.push_back(root_path);
  }
#endif  // defined(BAZEL_BUILD)

  for (size_t i = 0; i < root_paths.size(); ++i) {
    base::FilePath& root_path = root_paths[i];
    std::string root_path_canonicalized;
    base::ReplaceChars(root_path.MaybeAsASCII(), "\\", "/",
                       &root_path_canonicalized);
    source_tree.MapPath("", root_path_canonicalized);

    base::FileEnumerator enumerator(
        root_path, true, base::FileEnumerator::FILES,
        FILE_PATH_LITERAL("*.proto"),
        base::FileEnumerator::FolderSearchPolicy::ALL);

#if defined(BAZEL_BUILD)
    bool is_felicia_root = i == root_paths.size() - 1;
#endif  // defined(BAZEL_BUILD)

    for (auto path = enumerator.Next(); !path.empty();
         path = enumerator.Next()) {
      std::string path_canonicalized;
      base::ReplaceChars(path.MaybeAsASCII(), "\\", "/", &path_canonicalized);
      auto relative_path =
          path_canonicalized.substr(root_path_canonicalized.length());
#if defined(BAZEL_BUILD)
      if (is_felicia_root && !StartsWith(relative_path, "felicia/")) continue;
#endif  // defined(BAZEL_BUILD)
      descriptor_pool_->FindFileByName(relative_path);
    }
  }
}

bool ProtobufLoader::NewMessage(const std::string& type_name,
                                const google::protobuf::Message** message) {
  Load();

  const google::protobuf::Descriptor* descriptor =
      descriptor_pool_->FindMessageTypeByName(type_name);
  if (!descriptor) {
    LOG(ERROR) << "Failed to find message type: " << type_name
               << ". Maybe you forget to add the path to your protobuf to "
                  "FEL_PROTOBUF_ROOT_PATH?";
    return false;
  }

  *message = message_factory_.GetPrototype(descriptor);
  return true;
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