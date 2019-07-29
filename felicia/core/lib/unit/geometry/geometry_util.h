#ifndef FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_
#define FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

EXPORT Vec3fMessage EigenVec3fToVec3fMessage(const ::Eigen::Vector3f& vec);
EXPORT Vec3dMessage EigenVec3dToVec3dMessage(const ::Eigen::Vector3d& vec);

EXPORT QuarternionfMessage
EigenQuarternionfToQuarternionfMessage(const ::Eigen::Quaternionf& quarternion);
EXPORT QuarterniondMessage
EigenQuarterniondToQuarterniondMessage(const ::Eigen::Quaterniond& quarternion);

EXPORT PointfMessage PointfToPointfMessage(const Pointf& point);
EXPORT PointdMessage PointdToPointdMessage(const Pointd& point);

EXPORT Point3fMessage Point3fToPoint3fMessage(const Point3f& point);
EXPORT Point3dMessage Point3dToPoint3dMessage(const Point3d& point);

EXPORT PosefMessage PosefToPosefMessage(const Posef& pose);
EXPORT PosedMessage PosedToPosedMessage(const Posed& pose);

EXPORT Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose);
EXPORT Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_GEOMETRY_GEOMETRY_UTIL_H_