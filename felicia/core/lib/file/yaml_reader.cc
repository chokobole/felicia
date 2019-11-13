// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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