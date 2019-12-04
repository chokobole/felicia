// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/map/pointcloud.h"

#if defined(HAS_ROS)
#include <sensor_msgs/point_cloud2_iterator.h>
#endif  // defined(HAS_ROS)

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/file_util.h"
#include "felicia/core/lib/unit/time_util.h"

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

#if defined(HAS_ROS)
namespace {

struct PointFields {
  PointFields() : offset(0) {}

  void Add(const std::string& name, int count, int data_type) {
    sensor_msgs::PointField point_field;
    point_field.name = name;
    point_field.count = count;
    point_field.datatype = data_type;
    point_field.offset = offset;
    point_fields.push_back(point_field);

    offset += count * sizeOfPointField(data_type);
  }

  void AddPadding(int count, int data_type) {
    offset += count * sizeOfPointField(data_type);
  }

  int offset;
  std::vector<sensor_msgs::PointField> point_fields;
};

}  // namespace

bool Pointcloud::ToRosPointcloud(sensor_msgs::PointCloud2* pointcloud,
                                 int option) const {
#define GET_DATA_INFO(data)                                                 \
  internal::GetElementaAndChannelType(data##_.type(), &data##_element_type, \
                                      &data##_channel_type);                \
  data##_channel_size = internal::GetChannelSize(data##_channel_type)

  DataMessage::ElementType points_element_type, colors_element_type,
      intensities_element_type;
  DataMessage::ChannelType points_channel_type, colors_channel_type,
      intensities_channel_type;
  size_t points_channel_size, colors_channel_size, intensities_channel_size;
  PointFields point_fields;

  GET_DATA_INFO(points);
  if (points_element_type != DataMessage::ELEMENT_TYPE_32S &&
      points_element_type != DataMessage::ELEMENT_TYPE_32F &&
      points_element_type != DataMessage::ELEMENT_TYPE_64F) {
    return false;
  }

  if (points_channel_size == 2 || points_channel_size == 3) {
    point_fields.Add("x", 1, sensor_msgs::PointField::FLOAT32);
    point_fields.Add("y", 1, sensor_msgs::PointField::FLOAT32);
    point_fields.Add("z", 1, sensor_msgs::PointField::FLOAT32);
  } else {
    return false;
  }

  if (option & WITH_COLORS && colors_.size() > 0) {
    GET_DATA_INFO(colors);
    if (colors_element_type != DataMessage::ELEMENT_TYPE_8U &&
        colors_element_type != DataMessage::ELEMENT_TYPE_32F) {
      return false;
    }

    if (colors_channel_size == 3) {
      point_fields.Add("rgb", 1, sensor_msgs::PointField::UINT32);
    } else if (colors_channel_size == 4) {
      point_fields.Add("rgba", 1, sensor_msgs::PointField::UINT32);
    } else {
      return false;
    }
  }

  if (option & WITH_INTENCITIES && intensities_.size() > 0) {
    GET_DATA_INFO(intensities);
    if (intensities_element_type != DataMessage::ELEMENT_TYPE_8U &&
        intensities_element_type != DataMessage::ELEMENT_TYPE_16U) {
      return false;
    }

    if (intensities_channel_size == 1) {
      point_fields.Add("intensity", 1, sensor_msgs::PointField::UINT32);
    } else {
      return false;
    }
  }

  pointcloud->fields = std::move(point_fields.point_fields);
  if (points_channel_size == 2) {
    if (points_element_type == DataMessage::ELEMENT_TYPE_32S) {
      pointcloud->width = points_.AsConstView<Pointi>().size();
    } else if (points_element_type == DataMessage::ELEMENT_TYPE_32F) {
      pointcloud->width = points_.AsConstView<Pointf>().size();
    } else {
      pointcloud->width = points_.AsConstView<Pointd>().size();
    }
  } else {
    if (points_element_type == DataMessage::ELEMENT_TYPE_32S) {
      pointcloud->width = points_.AsConstView<Point3i>().size();
    } else if (points_element_type == DataMessage::ELEMENT_TYPE_32F) {
      pointcloud->width = points_.AsConstView<Point3f>().size();
    } else {
      pointcloud->width = points_.AsConstView<Point3d>().size();
    }
  }
  pointcloud->height = 1;
  pointcloud->point_step = point_fields.offset;
  pointcloud->row_step = pointcloud->width * pointcloud->point_step;
  pointcloud->data.resize(pointcloud->height * pointcloud->row_step);

  pointcloud->header.stamp = ToRosTime(timestamp_);
  uint8_t* dst = pointcloud->data.data();

  if (points_channel_size == 2) {
#define COPY_POINTS(type)                                                     \
  Data::ConstView<type> points_view = points_.AsConstView<type>();            \
  for (const type& p : points_view) {                                         \
    float values[2] = {static_cast<float>(p.x()), static_cast<float>(p.y())}; \
    memcpy(dst, values, 2 * sizeof(float));                                   \
    dst += pointcloud->point_step;                                            \
  }
    if (points_element_type == DataMessage::ELEMENT_TYPE_32S) {
      COPY_POINTS(Pointi)
    } else if (points_element_type == DataMessage::ELEMENT_TYPE_32F) {
      COPY_POINTS(Pointf)
    } else {
      COPY_POINTS(Pointd)
    }
#undef COPY_POINTS
  } else {
#define COPY_POINTS3(type)                                                   \
  Data::ConstView<type> points_view = points_.AsConstView<type>();           \
  for (const type& p : points_view) {                                        \
    float values[3] = {static_cast<float>(p.x()), static_cast<float>(p.y()), \
                       static_cast<float>(p.z())};                           \
    memcpy(dst, values, 3 * sizeof(float));                                  \
    dst += pointcloud->point_step;                                           \
  }
    if (points_element_type == DataMessage::ELEMENT_TYPE_32S) {
      COPY_POINTS3(Point3i)
    } else if (points_element_type == DataMessage::ELEMENT_TYPE_32F) {
      COPY_POINTS3(Point3f)
    } else {
      COPY_POINTS3(Point3d)
    }
#undef COPY_POINTS3
  }

  if (option & WITH_COLORS && colors_.size() > 0) {
    dst = pointcloud->data.data() + 3 * sizeof(float);
    if (colors_channel_size == 4) {
      if (colors_element_type == DataMessage::ELEMENT_TYPE_32F) {
        Data::ConstView<Color4f> colors_view = colors_.AsConstView<Color4f>();
        for (const Color4f& c : colors_view) {
          *dst = static_cast<uint8_t>(c.b() * 255);
          *(dst + 1) = static_cast<uint8_t>(c.g() * 255);
          *(dst + 2) = static_cast<uint8_t>(c.r() * 255);
          *(dst + 3) = static_cast<uint8_t>(c.a() * 255);
          dst += pointcloud->point_step;
        }
      } else {
        Data::ConstView<Color4u> colors_view = colors_.AsConstView<Color4u>();
        for (const Color4u& c : colors_view) {
          *dst = c.b();
          *(dst + 1) = c.g();
          *(dst + 2) = c.r();
          *(dst + 3) = c.a();
          dst += pointcloud->point_step;
        }
      }
    } else {
      if (colors_element_type == DataMessage::ELEMENT_TYPE_32F) {
        Data::ConstView<Color3f> colors_view = colors_.AsConstView<Color3f>();
        for (const Color3f& c : colors_view) {
          *dst = static_cast<uint8_t>(c.b() * 255);
          *(dst + 1) = static_cast<uint8_t>(c.g() * 255);
          *(dst + 2) = static_cast<uint8_t>(c.r() * 255);
          dst += pointcloud->point_step;
        }
      } else {
        Data::ConstView<Color3u> colors_view = colors_.AsConstView<Color3u>();
        for (const Color3u& c : colors_view) {
          *dst = c.b();
          *(dst + 1) = c.g();
          *(dst + 2) = c.r();
          dst += pointcloud->point_step;
        }
      }
    }
  }

  if (option & WITH_INTENCITIES && intensities_.size() > 0) {
    dst = pointcloud->data.data() + 3 * sizeof(float);
    if (option & WITH_COLORS) {
      dst += sizeof(uint32_t);
    }
    if (intensities_element_type == DataMessage::ELEMENT_TYPE_8U) {
      Data::ConstView<uint8_t> intensities_view =
          intensities_.AsConstView<uint8_t>();
      for (uint8_t i : intensities_view) {
        *dst = i;
        dst += pointcloud->point_step;
      }
    } else {
      Data::ConstView<uint16_t> intensities_view =
          intensities_.AsConstView<uint16_t>();
      for (uint16_t i : intensities_view) {
        *dst = static_cast<uint8_t>(i / 256);
        dst += pointcloud->point_step;
      }
    }
  }

  return true;
#undef GET_DATA_INFO
}

Status Pointcloud::FromRosPointcloud(const sensor_msgs::PointCloud2& pointcloud,
                                     int option) {
  Data points;
  Data intensities;
  Data colors;
  bool has_rgb = false;
  bool has_rgba = false;
  bool has_intensity = false;
  for (const sensor_msgs::PointField& field : pointcloud.fields) {
    if (field.name == "rgb") {
      has_rgb = true;
    } else if (field.name == "rgba") {
      has_rgba = true;
    } else if (field.name == "intensity") {
      has_intensity = true;
    }
  }

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  points.set_type(DATA_TYPE_32F_C3);
  Data::View<Point3f> points_view = points.AsView<Point3f>();
  points_view.resize(pointcloud.width);
  for (Point3f& p : points_view) {
    p.set_xyz(*iter_x, *iter_y, *iter_z);
    ++iter_x;
    ++iter_y;
    ++iter_z;
  }

  if (option & WITH_COLORS) {
    if (has_rgb) {
      sensor_msgs::PointCloud2ConstIterator<uint8_t> iter_color(pointcloud,
                                                                "rgb");
      colors.set_type(DATA_TYPE_8U_C3);
      Data::View<Color3u> colors_view = colors.AsView<Color3u>();
      colors_view.resize(pointcloud.width);
      for (Color3u& c : colors_view) {
        const uint8_t* c_ptr = &(*iter_color);
        c.set_rgb(c_ptr[2], c_ptr[1], c_ptr[0]);
        ++iter_color;
      }
    } else if (has_rgba) {
      sensor_msgs::PointCloud2ConstIterator<uint8_t> iter_color(pointcloud,
                                                                "rgba");
      colors.set_type(DATA_TYPE_8U_C4);
      Data::View<Color4u> colors_view = colors.AsView<Color4u>();
      colors_view.resize(pointcloud.width);
      for (Color4u& c : colors_view) {
        const uint8_t* c_ptr = &(*iter_color);
        c.set_rgba(c_ptr[2], c_ptr[1], c_ptr[0], c_ptr[3]);
        ++iter_color;
      }
    }
  }

  if (option & WITH_INTENCITIES) {
    if (has_intensity) {
      sensor_msgs::PointCloud2ConstIterator<uint8_t> iter_intensity(
          pointcloud, "intensity");
      intensities.set_type(DATA_TYPE_8U_C1);
      Data::View<uint8_t> intensities_view = intensities.AsView<uint8_t>();
      intensities_view.resize(pointcloud.width);
      for (uint8_t& i : intensities_view) {
        i = *iter_intensity;
        ++iter_intensity;
      }
    }
  }

  *this = Pointcloud{std::move(points), std::move(intensities),
                     std::move(colors), FromRosTime(pointcloud.header.stamp)};

  return Status::OK();
}
#endif  // defined(HAS_ROS)

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