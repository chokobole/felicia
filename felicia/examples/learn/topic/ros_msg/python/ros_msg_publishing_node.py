import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.core.protobuf.master_data_pb2 import TopicInfo

from felicia.examples.learn.topic.ros_msg._SimpleMessage import SimpleMessage


class RosMsgPublishingNode(fel.NodeLifecycle):
    def __init__(self, topic_create_flag):
        super().__init__()
        self.topic = topic_create_flag.topic_flag.value
        self.channel_def_type = ChannelDef.Type.Value(
            topic_create_flag.channel_type_flag.value)
        self.publisher = fel.communication.Publisher(
            SimpleMessage, TopicInfo.ROS)
        self.message_id = 0
        self.timestamper = fel.Timestamper()

    def on_did_create(self, node_info):
        self.node_info = node_info
        self.request_publish()

    def on_request_publish(self, status):
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
        self.repeating_publish()

    def request_publish(self):
        settings = fel.communication.Settings()
        settings.buffer_size = fel.Bytes.from_bytes(512)

        self.publisher.request_publish(self.node_info, self.topic, self.channel_def_type,
                                       settings, self.on_request_publish)

    def repeating_publish(self):
        self.publisher.publish(self.generate_message())

        if not self.publisher.is_unregistered():
            fel.MasterProxy.post_delayed_task(
                self.repeating_publish, fel.TimeDelta.from_seconds(1))

    def generate_message(self):
        message = SimpleMessage()
        timestamp = self.timestamper.timestamp()
        message.id = self.message_id
        message.timestamp = timestamp.in_microseconds()
        message.content = "hello world"
        self.message_id += 1
        return message
