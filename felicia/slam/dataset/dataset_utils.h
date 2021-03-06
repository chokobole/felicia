// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_SLAM_DATASET_DATASET_UTILS_H_
#define FELICIA_SLAM_DATASET_DATASET_UTILS_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/error/statusor.h"

namespace felicia {
namespace slam {

StatusOr<double> TryConvertToDouble(const std::string& str,
                                    const base::FilePath& path, int line);

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_DATASET_UTILS_H_