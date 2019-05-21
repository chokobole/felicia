#ifndef FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_
#define FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

EXPORT Vec3fMessage EigenVec3fToVec3fMessage(const ::Eigen::Vector3f& vec);

EXPORT QuarternionMessage
EigenQuarternionfToQuarternionMessage(const ::Eigen::Quaternionf& quarternion);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_