import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.python.node_lifecycle import NodeLifecycle

from felicia.examples.learn.message_communication.protobuf.message_spec_pb2 import MessageSpec


class SimplePublishigNode(NodeLifecycle):
    def __init__(self, topic, channel_type):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        if channel_type == 'TCP':
            self.channel_def.type = ChannelDef.TCP
        elif channel_type == 'UDP':
            self.channel_def.type = ChannelDef.UDP
        self.publisher = fel.Publisher()
        self.message_id = 0

    def on_init(self):
        print("SimpePublishingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimplePublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

        # fel.MasterProxy.post_delayed_task(
        #     self.request_unpublish, fel.from_seconds(10))

    def on_error(self, status):
        print("SimplePublishingNode.on_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_publish(self):
        self.publisher.request_publish(
            self.node_info, self.topic, self.channel_def, self.on_request_publish)

    def on_request_publish(self, status):
        print("SimplePublishingNode.on_request_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
        self.repeating_publish()

    def repeating_publish(self):
        self.publisher.publish(self.generate_message(), self.on_publish)

        if not self.publisher.is_unregistered():
            fel.MasterProxy.post_delayed_task(
                self.repeating_publish, fel.from_seconds(1))

    def on_publish(self, status):
        print("SimplePublishingNode.on_request_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def generate_message(self):
        message_spec = MessageSpec()
        message_spec.id = self.message_id
        message_spec.timestamp = fel.now().to_double_t()
        message_spec.content = "hello world"
        self.message_id += 1
        return message_spec

    def request_unpublish(self):
        print("SimplePublishingNode.request_unpublish()")
        self.publisher.request_unpublish(self.node_info, self.topic,
                                         self.on_request_unpublish)

    def on_request_unpublish(self, status):
        print("SimplePublishingNode.on_request_unpublish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
