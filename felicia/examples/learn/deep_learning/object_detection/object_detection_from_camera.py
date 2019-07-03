import sys

import numpy as np

# # Should be before import felicia_py
import felicia.examples.learn.message_communication.common.python.import_order_resolver
import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.bounding_box_pb2 import ImageWithBoundingBoxesMessage
from felicia.core.protobuf.master_data_pb2 import NodeInfo
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.drivers.camera.camera_frame_message_pb2 import CameraFrameMessage
from felicia.drivers.camera.camera_format_message_pb2 import PIXEL_FORMAT_RGB
from ssd_mobilenet import Inference


class ObjectDetectionNode(fel.NodeLifecycle):
    def __init__(self, topic, camera_descriptor):
        super().__init__()
        self.topic = topic
        self.camera_descriptor = camera_descriptor
        self.publisher = fel.communication.Publisher()
        self.draw_on_image = False

    def on_init(self):
        print("ObjectDetectionNode.on_init()")
        self.camera = fel.drivers.CameraFactory.new_camera(
            self.camera_descriptor)
        s = self.camera.init()
        if not s.ok():
            fel.log(fel.ERROR, s.error_message())
            sys.exit(1)
        self.inference = Inference()

    def on_did_create(self, node_info):
        print("ObjectDetectionNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

    def on_error(self, status):
        print("ObjectDetectionNode.on_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_publish(self):
        settings = fel.communication.Settings()
        settings.queue_size = 1
        settings.is_dynamic_buffer = True

        if self.draw_on_image:
            type_name = CameraFrameMessage.DESCRIPTOR.full_name
        else:
            type_name = ImageWithBoundingBoxesMessage.DESCRIPTOR.full_name

        self.publisher.request_publish(self.node_info, self.topic,
                                       ChannelDef.TCP | ChannelDef.WS,
                                       type_name, settings, self.on_request_publish)

    def on_request_publish(self, status):
        print("ObjectDetectionNode.on_request_publish()")
        if status.ok():
            fel.MasterProxy.post_task(self.start_camera)
        else:
            fel.log(fel.ERROR, status.error_message())

    def start_camera(self):
        # You should set the camera format if you have any you want to run with.
        s = self.camera.start(fel.drivers.CameraFormat(640, 480, PIXEL_FORMAT_RGB, 25),
                              self.on_camera_frame, self.on_camera_error)
        if not s.ok():
            fel.log(fel.ERROR, s.error_message())
            sys.exit(1)

    def on_camera_frame(self, camera_frame):
        if self.publisher.is_unregistered():
            return

        image_np = np.array(camera_frame, copy=False)

        if self.draw_on_image:
            detected_image = self.inference.run(image_np, self.draw_on_image)

            camera_format, timestamp = camera_frame
            detected_camera_frame = fel.drivers.CameraFrame(
                detected_image, camera_format, timestamp)

            self.publisher.publish(detected_camera_frame.to_camera_frame_message(),
                                   self.on_publish)
        else:
            image_with_bounding_boxes = self.inference.run(
                image_np, self.draw_on_image)

            self.publisher.publish(image_with_bounding_boxes, self.on_publish)

    def on_camera_error(self, status):
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_publish(self, channel_type, status):
        fel.log_if(fel.ERROR, not status.ok(), "{} from {}".format(
            status, ChannelDef.Type.Name(channel_type)))


def main():
    fel.felicia_init()

    camera_descriptors = []
    s = fel.drivers.CameraFactory.get_camera_descriptors(camera_descriptors)
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    if len(camera_descriptors) == 0:
        print("{} {}.".format(cli.RED_ERROR, "No camera device"), file=sys.stderr)
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()

    # Use the first camera
    fel.MasterProxy.request_register_node(
        ObjectDetectionNode, node_info, "message", camera_descriptors[0])

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
