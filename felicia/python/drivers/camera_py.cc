#include "felicia/python/drivers/camera_py.h"

#include "pybind11/stl.h"
#include "third_party/chromium/base/bind.h"

#include "felicia/drivers/camera/camera_factory.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_ENUM_TYPE_CAST(::felicia::PixelFormat, PixelFormat)

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::CameraFrameMessage, CameraFrameMessage,
                           felicia.drivers.camera.camera_frame_message_pb2)

namespace felicia {

using PyCameraFrameCallback = PyCallback<void(CameraFrame)>;

void AddCamera(py::module& m) {
  py::class_<CameraInterface, PyCameraInterface>(m, "CameraInterface")
      .def("init", &CameraInterface::Init)
      .def("start",
           [](CameraInterface& self, const CameraFormat& camera_format,
              py::function on_camera_frame_callback,
              py::function on_error_callback) {
             return self.Start(
                 camera_format,
                 ::base::BindRepeating(&PyCameraFrameCallback::Invoke,
                                       ::base::Owned(new PyCameraFrameCallback(
                                           on_camera_frame_callback))),
                 ::base::BindRepeating(
                     &PyStatusCallback::Invoke,
                     ::base::Owned(new PyStatusCallback(on_error_callback))));
           })
      .def("stop", &CameraInterface::Stop);

  py::class_<CameraDescriptor>(m, "CameraDescriptor")
      .def_property_readonly("display_name", &CameraDescriptor::display_name)
      .def_property_readonly("device_id", &CameraDescriptor::display_name)
      .def_property_readonly("model_id", &CameraDescriptor::model_id)
      .def("__str__", &CameraDescriptor::ToString);

  py::class_<CameraFormat>(m, "CameraFormat")
      .def(py::init([](int width, int height, PixelFormat pixel_format,
                       float frame_rate, bool convert_to_argb) {
        return new CameraFormat(width, height, pixel_format, frame_rate,
                                convert_to_argb);
      }))
      .def_property("pixel_format", &CameraFormat::pixel_format,
                    &CameraFormat::set_pixel_format)
      .def_property("width", &CameraFormat::width,
                    [](CameraFormat& self, int width) {
                      self.SetSize(width, self.height());
                    })
      .def_property("height", &CameraFormat::height,
                    [](CameraFormat& self, int height) {
                      self.SetSize(self.width(), height);
                    })
      .def_property("frame_rate", &CameraFormat::frame_rate,
                    &CameraFormat::set_frame_rate)
      .def_property("convert_to_argb", &CameraFormat::convert_to_argb,
                    &CameraFormat::set_convert_to_argb)
      .def("__str__", &CameraFormat::ToString);

  py::class_<CameraFrame>(m, "CameraFrame")
      .def_property_readonly("width", &CameraFrame::width)
      .def_property_readonly("height", &CameraFrame::height)
      .def_property_readonly("allocation_size", &CameraFrame::AllocationSize)
      .def_property_readonly("pixel_format", &CameraFrame::pixel_format)
      .def_property_readonly("timestamp", &CameraFrame::timestamp)
      .def("to_camera_frame_message", &CameraFrame::ToCameraFrameMessage);

  py::class_<CameraFactory>(m, "CameraFactory")
      .def_static("new_camera", &CameraFactory::NewCamera)
      .def_static(
          "get_camera_descriptors",
          [](py::list list) {
            CameraDescriptors camera_descriptors;
            Status s = CameraFactory::GetCameraDescriptors(&camera_descriptors);
            for (auto& camera_descriptor : camera_descriptors) {
              list.append(camera_descriptor);
            }
            return s;
          })
      .def_static("get_supported_camera_formats",
                  [](const CameraDescriptor& camera_descriptor, py::list list) {
                    CameraFormats camera_formats;
                    Status s = CameraFactory::GetSupportedCameraFormats(
                        camera_descriptor, &camera_formats);
                    for (auto& camera_format : camera_formats) {
                      list.append(camera_format);
                    }
                    return s;
                  });
}

}  // namespace felicia