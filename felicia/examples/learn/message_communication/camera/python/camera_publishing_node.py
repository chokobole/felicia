import sys

import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.drivers.camera.camera_frame_message_pb2 import CameraFrameMessage
from felicia.drivers.camera.camera_format_message_pb2 import PIXEL_FORMAT_YUY2


class CameraPublishingNode(fel.NodeLifecycle):
    def __init__(self, topic, channel_type, camera_descriptor, buffer_size):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        self.channel_def.type = ChannelDef.Type.Value(channel_type)
        self.camera_descriptor = camera_descriptor
        self.buffer_size = buffer_size
        self.publisher = fel.Publisher()

    def on_init(self):
        print("CameraPublishingNode.on_init()")
        self.camera = fel.CameraFactory.new_camera(self.camera_descriptor)
        fel.check(self.camera.init().ok())

    def on_did_create(self, node_info):
        print("CameraPublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

    def on_error(self, status):
        print("CameraPublishingNode.on_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_publish(self):
        settings = fel.Settings()
        settings.buffer_size = fel.Bytes.from_bytes(self.buffer_size)

        self.publisher.request_publish(
            self.node_info, self.topic, self.channel_def, settings, self.on_request_publish)

    def on_request_publish(self, status):
        print("CameraPublishingNode.on_request_publish()")
        if status.ok():
            fel.MasterProxy.post_task(self.start_camera)
        else:
            fel.log(fel.ERROR, status.error_message())

    def start_camera(self):
        # You should set the camera format if you have any you want to run with.
        s = self.camera.start(fel.CameraFormat(640, 480, PIXEL_FORMAT_YUY2, 25),
                              self.on_camera_frame, self.on_camera_error)
        if s.ok():
            # fel.MasterProxy.post_delayed_task(
            #     self.request_unpublish, fel.TimeDelta.from_seconds(10))
            pass
        else:
            fel.log(fel.ERROR, s.error_message())

    def on_camera_frame(self, camera_frame):
        print("CameraPublishingNode.on_camera_frame")
        if self.publisher.is_unregistered():
            return

        self.publisher.publish(camera_frame.to_camera_frame_message(),
                               self.on_publish)

    def on_camera_error(self, status):
        print("CameraPublishingNode.on_camera_error")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_publish(self, status):
        print("CameraPublishingNode.on_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_unpublish(self):
        self.publisher.request_unpublish(self.node_info, self.topic,
                                         self.on_request_unpublish)

    def on_request_unpublish(self, status):
        print("CameraPublishingNode.on_request_unpublish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def stop_camera(self):
        s = self.camera.stop()
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
