// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/map/pointcloud.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/file_util.h"

namespace felicia {
namespace map {

Pointcloud::Pointcloud() = default;

Pointcloud::Pointcloud(const Data& points, const Data& intensities,
                       const Data& colors, base::TimeDelta timestamp)
    : points_(points),
      intensities_(intensities),
      colors_(colors),
      timestamp_(timestamp) {}

Pointcloud::Pointcloud(Data&& points, Data&& intensities, Data&& colors,
                       base::TimeDelta timestamp) noexcept
    : points_(std::move(points)),
      intensities_(std::move(intensities)),
      colors_(std::move(colors)),
      timestamp_(timestamp) {}

Pointcloud::Pointcloud(const Pointcloud& other)
    : points_(other.points_),
      intensities_(other.intensities_),
      colors_(other.colors_),
      timestamp_(other.timestamp_) {}

Pointcloud::Pointcloud(Pointcloud&& other) noexcept
    : points_(std::move(other.points_)),
      intensities_(std::move(other.intensities_)),
      colors_(std::move(other.colors_)),
      timestamp_(other.timestamp_) {}

Pointcloud& Pointcloud::operator=(const Pointcloud& other) = default;
Pointcloud& Pointcloud::operator=(Pointcloud&& other) = default;

Pointcloud::~Pointcloud() = default;

const Data& Pointcloud::points() const { return points_; }

Data& Pointcloud::points() { return points_; }

const Data& Pointcloud::intensities() const { return intensities_; }

Data& Pointcloud::intensities() { return intensities_; }

const Data& Pointcloud::colors() const { return colors_; }

Data& Pointcloud::colors() { return colors_; }

void Pointcloud::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta Pointcloud::timestamp() const { return timestamp_; }

PointcloudMessage Pointcloud::ToPointcloudMessage(bool copy, int option) {
  PointcloudMessage message;
  *message.mutable_points() = points_.ToDataMessage(copy);
  if (option & WITH_INTENCITIES)
    *message.mutable_intensities() = intensities_.ToDataMessage(copy);
  if (option & WITH_COLORS)
    *message.mutable_colors() = colors_.ToDataMessage(copy);
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

Status Pointcloud::FromPointcloudMessage(const PointcloudMessage& message,
                                         int option) {
  Data points;
  Data intensities;
  Data colors;
  Status s = points.FromDataMessage(message.points());
  if (!s.ok()) return s;

  if (option & WITH_INTENCITIES) {
    s = intensities.FromDataMessage(message.intensities());
    if (!s.ok()) return s;
  }

  if (option & WITH_COLORS) {
    s = colors.FromDataMessage(message.colors());
    if (!s.ok()) return s;
  }

  *this =
      Pointcloud{std::move(points), std::move(intensities), std::move(colors),
                 base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

Status Pointcloud::FromPointcloudMessage(PointcloudMessage&& message,
                                         int option) {
  Data points;
  Data intensities;
  Data colors;
  std::unique_ptr<DataMessage> points_message(message.release_points());
  Status s = points.FromDataMessage(std::move(*points_message));
  if (!s.ok()) return s;

  if (option & WITH_INTENCITIES) {
    std::unique_ptr<DataMessage> intensities_message(
        message.release_intensities());
    s = intensities.FromDataMessage(std::move(*intensities_message));
    if (!s.ok()) return s;
  }

  if (option & WITH_COLORS) {
    std::unique_ptr<DataMessage> colors_message(message.release_colors());
    s = colors.FromDataMessage(std::move(*colors_message));
    if (!s.ok()) return s;
  }

  *this =
      Pointcloud{std::move(points), std::move(intensities), std::move(colors),
                 base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

Status Pointcloud::Load(const base::FilePath& path, int option) {
  std::unique_ptr<char[]> buffer;
  size_t len;
  if (!ReadFile(path, &buffer, &len)) {
    return errors::InvalidArgument(base::StringPrintf(
        "Failed to read the file %" PRFilePath, path.value().c_str()));
  }
  if (len % 16 != 0) {
    return errors::InvalidArgument("length is not a multiple of 16.");
  }

  Data points;
  Data intensities;
  points.set_type(DATA_TYPE_32F_C3);
  intensities.set_type(DATA_TYPE_32F_C1);
  Data::View<Point3f> points_vector = points.AsView<Point3f>();
  Data::View<float> intensities_vector = intensities.AsView<float>();
  base::FilePath::StringType extension = path.Extension();
  if (extension == FILE_PATH_LITERAL(".bin")) {
    const float* p = reinterpret_cast<const float*>(buffer.get());
    if (option & WITH_INTENCITIES) {
      for (size_t i = 0; i < len; i += 16) {
        points_vector.emplace_back(*p, *(p + 1), *(p + 2));
        intensities_vector.push_back(*(p + 3));
        p = p + 4;
      }
      points_ = std::move(points);
      intensities_ = std::move(intensities);
      colors_.clear();
    } else {
      for (size_t i = 0; i < len; i += 16) {
        points_vector.emplace_back(*p, *(p + 1), *(p + 2));
        p = p + 4;
      }
      points_ = std::move(points);
      intensities_.clear();
      colors_.clear();
    }
    return Status::OK();
  }
  return errors::InvalidArgument(
      base::StringPrintf("Don't know how to handle with extension %" PRFilePath,
                         extension.c_str()));
}

Status Pointcloud::Save(const base::FilePath& path, int option) const {
  if (option & WITH_INTENCITIES) {
    CHECK_EQ(points_.size(), intensities_.size())
        << "You set WITH_INTENCITIES but the both size are not equal";
  }
  if (option & WITH_COLORS) {
    CHECK_EQ(points_.size(), colors_.size())
        << "You set WITH_COLORS but the both size are not equal";
  }
  std::stringstream ss;
  Data::ConstView<Point3f> points_vector = points_.AsConstView<Point3f>();
  Data::ConstView<float> intensities_vector = intensities_.AsConstView<float>();
  base::FilePath::StringType extension = path.Extension();
  if (extension == FILE_PATH_LITERAL(".bin")) {
    if (option & WITH_INTENCITIES) {
      for (size_t i = 0; i < points_.size(); ++i) {
        const Point3f& p = points_vector[i];
        ss << p.x() << p.y() << p.z() << intensities_vector[i];
      }
    } else {
      for (size_t i = 0; i < points_.size(); ++i) {
        const Point3f& p = points_vector[i];
        ss << p.x() << p.y() << p.z();
      }
    }
  } else {
    return errors::InvalidArgument(base::StringPrintf(
        "Don't know how to handle with extension %" PRFilePath,
        extension.c_str()));
  }
  std::string content = ss.str();
  if (WriteFile(path, content.c_str(), content.length())) return Status::OK();
  return errors::InvalidArgument(base::StringPrintf(
      "Failed to write the file %" PRFilePath, path.value().c_str()));
}

}  // namespace map
}  // namespace felicia