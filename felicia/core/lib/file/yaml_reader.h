#ifndef FELICIA_CORE_LIB_YAML_READER_H_
#define FELICIA_CORE_LIB_YAML_READER_H_

#include "yaml-cpp/yaml.h"

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class EXPORT YamlReader {
 public:
  YamlReader();

  Status Open(const base::FilePath& path);

  template <typename Key>
  const YAML::Node operator[](const Key& key) const {
    return node_[key];
  }
  template <typename Key>
  YAML::Node operator[](const Key& key) {
    return node_[key];
  }

  const YAML::Node operator[](const YAML::Node& key) const;
  YAML::Node operator[](const YAML::Node& key);

 private:
  YAML::Node node_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_YAML_READER_H_