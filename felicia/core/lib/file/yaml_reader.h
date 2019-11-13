#ifndef FELICIA_CORE_LIB_YAML_READER_H_
#define FELICIA_CORE_LIB_YAML_READER_H_

#include "yaml-cpp/yaml.h"

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT YamlReader {
 public:
  YamlReader();
  ~YamlReader();

  Status Open(const base::FilePath& path);

  const YAML::Node& node() const;
  YAML::Node& node();

 private:
  YAML::Node node_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_YAML_READER_H_