from felicia.core.channel.channel_pb2 import ChannelDef
from felicia.examples.learn.message_communication.protobuf.message_spec_pb2 import MessageSpec
from felicia.examples.learn.message_communication.python.communication import Publisher
from felicia.python.core.node.node_lifecycle import NodeLifecycle


class SimplePublishigNode(NodeLifecycle):
    def __init__(self, topic, channel_type):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        if channel_type == 'TCP':
            self.channel_def.type = ChannelDef.TCP
        elif channel_type == 'UDP':
            self.channel_def.type = ChannelDef.UDP
        self.publisher = Publisher()
        self.message_id = 0

    def on_init(self):
        print("SimpePublishingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimplePublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

    def on_error(self, status):
        print("SimplePublishingNode.on_error()")
        if not status.ok():
            print(status.error_message())

    def request_publish(self):
        self.publisher.request_publish(
            self.node_info, self.topic, self.channel_def, self.on_request_publish)

    def on_request_publish(self, status):
        print("SimplePublishingNode.on_request_publish")
        if not status.ok():
            print(status.error_message())
        self.repeating_publish()

    def repeating_publish(self):
        self.publisher.publish(self.generate_message(), self.on_publish)

    def on_publish(self, status):
        print("SimplePublishingNode.on_request_publish")
        if not status.ok():
            print(status.error_message())

    def generate_message(self):
        message_spec = MessageSpec()
        message_spec.id = self.message_id
        message_spec.content = "hello world"
        self.message_id += 1
        return message_spec
