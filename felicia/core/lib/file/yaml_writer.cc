// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/yaml_writer.h"

#include "third_party/chromium/base/files/file.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

YamlWriter::YamlWriter() = default;

YamlWriter::~YamlWriter() = default;

Status YamlWriter::WriteToFile(const base::FilePath& path) {
  base::File file(path,
                  base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  if (!file.IsValid())
    return errors::InvalidArgument(
        base::File::ErrorToString(file.error_details()));
  file.WriteAtCurrentPos(emitter_.c_str(), emitter_.size());
  return Status::OK();
}

YAML::Emitter& YamlWriter::emitter() { return emitter_; }

}  // namespace felicia