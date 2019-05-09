#include "pybind11/pybind11.h"

#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/python/communication_py_helper.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::CameraFrameMessage, CameraFrameMessage,
                           felicia.drivers.camera.camera_frame_message_pb2)

namespace py = pybind11;

namespace felicia {

void AddCommunication(py::module& m) {
  AddPublisher<CameraFrameMessage>(m, "Publisher");
  AddSubscriber<CameraFrameMessage>(m, "Subscriber");
}

}  // namespace felicia