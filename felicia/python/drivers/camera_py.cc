#include "felicia/python/drivers/camera_py.h"

#include "pybind11/numpy.h"
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

namespace {

void NotHaveFixedSizedChannelPixelFormat() {
  PyErr_SetString(PyExc_TypeError,
                  "pixel_format doens't have a fixed sized channel.");
  throw py::error_already_set();
}

}  // namespace

void AddCamera(py::module& m) {
  py::class_<CameraSettings>(m, "CameraSettings")
      .def(py::init<>())
      .def("has_white_balance_mode", &CameraSettings::has_white_balance_mode)
      .def("set_white_balance_mode", &CameraSettings::set_white_balance_mode)
      .def("white_balance_mode", &CameraSettings::white_balance_mode)
      .def("has_exposure_mode", &CameraSettings::has_exposure_mode)
      .def("set_exposure_mode", &CameraSettings::set_exposure_mode)
      .def("exposure_mode", &CameraSettings::exposure_mode)
      .def("has_exposure_compensation",
           &CameraSettings::has_exposure_compensation)
      .def("set_exposure_compensation",
           &CameraSettings::set_exposure_compensation)
      .def("exposure_compensation", &CameraSettings::exposure_compensation)
      .def("has_exposure_time", &CameraSettings::has_exposure_time)
      .def("set_exposure_time", &CameraSettings::set_exposure_time)
      .def("exposure_time", &CameraSettings::exposure_time)
      .def("has_color_temperature", &CameraSettings::has_color_temperature)
      .def("set_color_temperature", &CameraSettings::set_color_temperature)
      .def("color_temperature", &CameraSettings::color_temperature)
      .def("has_brightness", &CameraSettings::has_brightness)
      .def("set_brightness", &CameraSettings::set_brightness)
      .def("brightness", &CameraSettings::brightness)
      .def("has_contrast", &CameraSettings::has_contrast)
      .def("set_contrast", &CameraSettings::set_contrast)
      .def("contrast", &CameraSettings::contrast)
      .def("has_saturation", &CameraSettings::has_saturation)
      .def("set_saturation", &CameraSettings::set_saturation)
      .def("saturation", &CameraSettings::saturation)
      .def("has_sharpness", &CameraSettings::has_sharpness)
      .def("set_sharpness", &CameraSettings::set_sharpness)
      .def("sharpness", &CameraSettings::sharpness)
      .def("has_hue", &CameraSettings::has_hue)
      .def("set_hue", &CameraSettings::set_hue)
      .def("hue", &CameraSettings::hue)
      .def("has_gain", &CameraSettings::has_gain)
      .def("set_gain", &CameraSettings::set_gain)
      .def("gain", &CameraSettings::gain)
      .def("has_gamma", &CameraSettings::has_gamma)
      .def("set_gamma", &CameraSettings::set_gamma)
      .def("gamma", &CameraSettings::gamma);

  py::class_<CameraInterface, PyCameraInterface>(m, "CameraInterface")
      .def("init", &CameraInterface::Init,
           py::call_guard<py::gil_scoped_release>())
      .def(
          "start",
          [](CameraInterface& self, const CameraFormat& camera_format,
             py::function on_camera_frame_callback,
             py::function on_error_callback) {
            return self.Start(
                camera_format,
                base::BindRepeating(&PyCameraFrameCallback::Invoke,
                                    base::Owned(new PyCameraFrameCallback(
                                        on_camera_frame_callback))),
                base::BindRepeating(
                    &PyStatusCallback::Invoke,
                    base::Owned(new PyStatusCallback(on_error_callback))));
          },
          py::call_guard<py::gil_scoped_release>())
      .def("stop", &CameraInterface::Stop,
           py::call_guard<py::gil_scoped_release>())
      .def("set_camera_settings", &CameraInterface::SetCameraSettings)
      .def("get_camera_settings_info",
           [](CameraInterface& self, py::object object) {
             CameraSettingsInfoMessage message;
             Status s = self.GetCameraSettingsInfo(&message);

             std::string text;
             message.SerializeToString(&text);
             object.attr("ParseFromString")(py::bytes(text));
             return s;
           })
      .def("is_initialized", &CameraInterface::IsInitialized)
      .def("is_started", &CameraInterface::IsStarted)
      .def("is_stopped", &CameraInterface::IsStopped)
      .def("camera_format", &CameraInterface::camera_format);

  py::class_<CameraDescriptor>(m, "CameraDescriptor")
      .def_property_readonly("display_name", &CameraDescriptor::display_name)
      .def_property_readonly("device_id", &CameraDescriptor::display_name)
      .def_property_readonly("model_id", &CameraDescriptor::model_id)
      .def("__str__", &CameraDescriptor::ToString);

  py::class_<CameraFormat>(m, "CameraFormat")
      .def(py::init([](int width, int height, PixelFormat pixel_format,
                       float frame_rate) {
        return new CameraFormat(width, height, pixel_format, frame_rate);
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
      .def("__str__", &CameraFormat::ToString);

  py::class_<CameraFrame>(m, "CameraFrame", py::buffer_protocol())
      .def(py::init(
          [](py::array_t<uint8_t, py::array::c_style | py::array::forcecast>
                 array,
             const CameraFormat& camera_format, base::TimeDelta timestamp) {
            if (!camera_format.HasFixedSizedChannelPixelFormat()) {
              NotHaveFixedSizedChannelPixelFormat();
            }

            std::unique_ptr<uint8_t[]> data(new uint8_t[array.size()]);
            memcpy(data.get(), array.data(), array.size());
            return CameraFrame(std::move(data), array.size(), camera_format,
                               timestamp);
          }))
      .def_property_readonly("length", &CameraFrame::length)
      .def_property_readonly("camera_format", &CameraFrame::camera_format)
      .def_property_readonly("width", &CameraFrame::width)
      .def_property_readonly("height", &CameraFrame::height)
      .def_property_readonly("pixel_format", &CameraFrame::pixel_format)
      .def_property_readonly("frame_rate", &CameraFrame::frame_rate)
      .def_property_readonly("timestamp", &CameraFrame::timestamp)
      .def("to_camera_frame_message", &CameraFrame::ToCameraFrameMessage)
      .def_buffer([](CameraFrame& camera_frame) {
        if (!camera_frame.camera_format().HasFixedSizedChannelPixelFormat()) {
          NotHaveFixedSizedChannelPixelFormat();
        }

        int width = camera_frame.width();
        int height = camera_frame.height();
        int channel = camera_frame.length() / (width * height);
        return py::buffer_info(const_cast<uint8_t*>(camera_frame.data_ptr()),
                               sizeof(uint8_t),
                               py::format_descriptor<uint8_t>::format(), 3,
                               {height, width, channel},
                               {sizeof(uint8_t) * width * channel,
                                sizeof(uint8_t) * channel, sizeof(uint8_t)});
      });

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