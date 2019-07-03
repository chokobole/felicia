import sys

import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.drivers.camera.camera_frame_message_pb2 import CameraFrameMessage
from felicia.drivers.camera.camera_format_message_pb2 import PIXEL_FORMAT_BGR
from felicia.drivers.camera.camera_settings_message_pb2 import CameraSettingsInfoMessage


class CameraPublishingNode(fel.NodeLifecycle):
    def __init__(self, topic, camera_descriptor):
        super().__init__()
        self.topic = topic
        self.camera_descriptor = camera_descriptor
        self.publisher = fel.communication.Publisher()

    def on_init(self):
        print("CameraPublishingNode.on_init()")
        self.camera = fel.drivers.CameraFactory.new_camera(
            self.camera_descriptor)
        s = self.camera.init()
        if not s.ok():
            fel.log(fel.ERROR, s.error_message())
            sys.exit(1)

        # You can set camera settings here.
        camera_settings = fel.drivers.CameraSettings()
        s = self.camera.set_camera_settings(camera_settings)
        fel.log_if(fel.ERROR, not s.ok(), s.error_message())

        message = CameraSettingsInfoMessage()
        s = self.camera.get_camera_settings_info(message)
        if s.ok():
            print(message)
        else:
            fel.log(fel.ERROR, s.error_message())

    def on_did_create(self, node_info):
        print("CameraPublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

    def on_error(self, status):
        print("CameraPublishingNode.on_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_publish(self):
        settings = fel.communication.Settings()
        settings.queue_size = 1
        settings.is_dynamic_buffer = True
        self.publisher.request_publish(self.node_info, self.topic,
                                       ChannelDef.TCP | ChannelDef.WS,
                                       CameraFrameMessage.DESCRIPTOR.full_name,
                                       settings, self.on_request_publish)

    def on_request_publish(self, status):
        print("CameraPublishingNode.on_request_publish()")
        if status.ok():
            fel.MasterProxy.post_task(self.start_camera)
        else:
            fel.log(fel.ERROR, status.error_message())

    def start_camera(self):
        # You should set the camera format if you have any you want to run with.
        s = self.camera.start(fel.drivers.CameraFormat(640, 480, PIXEL_FORMAT_BGR, 25),
                              self.on_camera_frame, self.on_camera_error)
        if s.ok():
            print("Camera format: {}".format(self.camera.camera_format()))
            # fel.MasterProxy.post_delayed_task(
            #     self.request_unpublish, fel.TimeDelta.from_seconds(10))
            pass
        else:
            fel.log(fel.ERROR, s.error_message())

    def on_camera_frame(self, camera_frame):
        if self.publisher.is_unregistered():
            return

        self.publisher.publish(camera_frame.to_camera_frame_message(),
                               self.on_publish)

    def on_camera_error(self, status):
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_publish(self, channel_type, status):
        fel.log_if(fel.ERROR, not status.ok(), "{} from {}".format(
            status, ChannelDef.Type.Name(channel_type)))

    def request_unpublish(self):
        self.publisher.request_unpublish(self.node_info, self.topic,
                                         self.on_request_unpublish)

    def on_request_unpublish(self, status):
        print("CameraPublishingNode.on_request_unpublish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def stop_camera(self):
        s = self.camera.stop()
        fel.log_if(fel.ERROR, not s.ok(), s.error_message())
