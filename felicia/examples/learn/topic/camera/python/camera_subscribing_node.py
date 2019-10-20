import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.drivers.camera.camera_frame_message_pb2 import CameraFrameMessage


class CameraSubscribingNode(fel.NodeLifecycle):
    def __init__(self, camera_flag):
        super().__init__()
        self.camera_flag = camera_flag
        self.topic = camera_flag.topic_flag.value
        self.subscriber = fel.communication.Subscriber(CameraFrameMessage)

    def on_did_create(self, node_info):
        self.node_info = node_info
        self.request_subscribe()

    def on_message(self, message):
        print("CameraSubscribingNode.on_message()")

    def request_subscribe(self):
        settings = fel.communication.Settings()
        settings.period = fel.TimeDelta.from_seconds_d(
            1 / self.camera_flag.fps_flag.value)
        settings.is_dynamic_buffer = True

        self.subscriber.request_subscribe(self.node_info, self.topic,
                                          ChannelDef.CHANNEL_TYPE_TCP |
                                          ChannelDef.CHANNEL_TYPE_SHM,
                                          settings, self.on_message)
