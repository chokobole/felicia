import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef

from felicia.examples.learn.message_communication.protobuf.message_spec_pb2 import MessageSpec


class SimplePublishingNode(fel.NodeLifecycle):
    def __init__(self, topic, channel_type):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        self.channel_def.type = ChannelDef.Type.Value(channel_type)
        self.publisher = fel.Publisher()
        self.message_id = 0

    def on_init(self):
        print("SimplePublishingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimplePublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

        # fel.MasterProxy.post_delayed_task(
        #     self.request_unpublish, fel.TimeDelta.from_seconds(10))

    def on_error(self, status):
        print("SimplePublishingNode.on_error()")
        fel.log(fel.ERROR, status.error_message())

    def request_publish(self):
        settings = fel.Settings()
        settings.buffer_size = fel.Bytes.from_bytes(512)

        self.publisher.request_publish(
            self.node_info, self.topic, self.channel_def, settings,
            self.on_request_publish)

    def on_request_publish(self, status):
        print("SimplePublishingNode.on_request_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
        self.repeating_publish()

    def repeating_publish(self):
        self.publisher.publish(self.generate_message(), self.on_publish)

        if not self.publisher.is_unregistered():
            fel.MasterProxy.post_delayed_task(
                self.repeating_publish, fel.TimeDelta.from_seconds(1))

    def on_publish(self, status):
        print("SimplePublishingNode.on_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def generate_message(self):
        message_spec = MessageSpec()
        message_spec.id = self.message_id
        message_spec.timestamp = fel.Time.now().to_double_t()
        message_spec.content = "hello world"
        self.message_id += 1
        return message_spec

    def request_unpublish(self):
        self.publisher.request_unpublish(self.node_info, self.topic,
                                         self.on_request_unpublish)

    def on_request_unpublish(self, status):
        print("SimplePublishingNode.on_request_unpublish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
