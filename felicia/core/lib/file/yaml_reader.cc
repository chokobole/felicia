#include "felicia/core/lib/file/yaml_reader.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

YamlReader::YamlReader() = default;

Status YamlReader::Open(const base::FilePath& path) {
  try {
    node_ = YAML::LoadFile(path.AsUTF8Unsafe());
  } catch (YAML::Exception e) {
    return errors::Unknown(e.what());
  }
  return Status::OK();
}

const YAML::Node YamlReader::operator[](const YAML::Node& key) const {
  return node_[key];
}

YAML::Node YamlReader::operator[](const YAML::Node& key) { return node_[key]; }

}  // namespace felicia