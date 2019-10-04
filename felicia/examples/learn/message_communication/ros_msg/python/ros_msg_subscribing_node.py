import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.core.protobuf.master_data_pb2 import TopicInfo

from felicia.examples.learn.message_communication.ros_msg._SimpleMessage import SimpleMessage


class RosMsgSubscribingNode(fel.NodeLifecycle):
    def __init__(self, node_create_flag):
        super().__init__()
        self.topic = node_create_flag.topic_flag.value
        self.subscriber = fel.communication.Subscriber()
        self.subscriber.set_message_impl_type(TopicInfo.ROS)

    def on_did_create(self, node_info):
        self.node_info = node_info
        self.request_subscribe()

    def on_message(self, message):
        print("message : {}".format(message))

    def request_subscribe(self):
        settings = fel.communication.Settings()
        settings.buffer_size = fel.Bytes.from_bytes(512)

        self.subscriber.request_subscribe(self.node_info, self.topic,
                                          ChannelDef.CHANNEL_TYPE_TCP |
                                          ChannelDef.CHANNEL_TYPE_UDP |
                                          ChannelDef.CHANNEL_TYPE_UDS |
                                          ChannelDef.CHANNEL_TYPE_SHM,
                                          SimpleMessage, settings,
                                          self.on_message)
