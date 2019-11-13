#ifndef FELICIA_CORE_LIB_YAML_WRITER_H_
#define FELICIA_CORE_LIB_YAML_WRITER_H_

#include "yaml-cpp/yaml.h"

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT YamlWriter {
 public:
  YamlWriter();
  ~YamlWriter();

  Status WriteToFile(const base::FilePath& path);

  YAML::Emitter& emitter();

 private:
  YAML::Emitter emitter_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_YAML_WRITER_H_