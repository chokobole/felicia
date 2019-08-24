#include "felicia/core/lib/file/yaml_reader.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

YamlReader::YamlReader() = default;

YamlReader::~YamlReader() = default;

Status YamlReader::Open(const base::FilePath& path) {
  try {
    node_ = YAML::LoadFile(path.AsUTF8Unsafe());
  } catch (YAML::Exception e) {
    return errors::Unknown(e.what());
  }
  return Status::OK();
}

const YAML::Node& YamlReader::node() const { return node_; }
YAML::Node& YamlReader::node() { return node_; }

}  // namespace felicia