#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

template <typename MessageType, typename PointType>
MessageType PointToPointMessage(const PointType& point) {
  MessageType message;
  message.set_x(point.x());
  message.set_y(point.y());
  return message;
}

PointfMessage PointfToPointfMessage(const Pointf& point) {
  return PointToPointMessage<PointfMessage>(point);
}

PointdMessage PointdToPointdMessage(const Pointd& point) {
  return PointToPointMessage<PointdMessage>(point);
}

template <typename PointType, typename MessageType>
PointType PointMessageToPoint(const MessageType& message) {
  return {message.x(), message.y()};
}

Pointf PointfMessageToPointf(const PointfMessage& message) {
  return PointMessageToPoint<Pointf>(message);
}

Pointd PointdMessageToPointd(const PointdMessage& message) {
  return PointMessageToPoint<Pointd>(message);
}

template <typename MessageType, typename PointType>
MessageType Point3ToPoint3Message(const PointType& point) {
  MessageType message;
  message.set_x(point.x());
  message.set_y(point.y());
  message.set_z(point.z());
  return message;
}

Point3fMessage Point3fToPoint3fMessage(const Point3f& point) {
  return Point3ToPoint3Message<Point3fMessage>(point);
}

Point3dMessage Point3dToPoint3dMessage(const Point3d& point) {
  return Point3ToPoint3Message<Point3dMessage>(point);
}

template <typename PointType, typename MessageType>
PointType Point3MessageToPoint3(const MessageType& message) {
  return {message.x(), message.y(), message.z()};
}

Point3f Point3fMessageToPoint3f(const Point3fMessage& message) {
  return Point3MessageToPoint3<Point3f>(message);
}

Point3d Point3dMessageToPoint3d(const Point3dMessage& message) {
  return Point3MessageToPoint3<Point3d>(message);
}

}  // namespace felicia