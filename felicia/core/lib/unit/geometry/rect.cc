#include "felicia/core/lib/unit/geometry/rect.h"

namespace felicia {

RectiMessage RectiToRectiMessage(const Recti& rect) {
  return RectToRectMessage<RectiMessage, PointiMessage>(rect);
}

RectfMessage RectfToRectfMessage(const Rectf& rect) {
  return RectToRectMessage<RectfMessage, PointfMessage>(rect);
}

RectdMessage RectdToRectdMessage(const Rectd& rect) {
  return RectToRectMessage<RectdMessage, PointdMessage>(rect);
}

Recti RectiMessageToRecti(const RectiMessage& message) {
  return RectMessageToRect<int>(message);
}

Rectf RectfMessageToRectf(const RectfMessage& message) {
  return RectMessageToRect<float>(message);
}

Rectd RectdMessageToRectd(const RectdMessage& message) {
  return RectMessageToRect<double>(message);
}

}  // namespace felicia