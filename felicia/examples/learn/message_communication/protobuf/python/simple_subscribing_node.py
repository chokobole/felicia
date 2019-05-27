import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef

from felicia.examples.learn.message_communication.protobuf.message_spec_pb2 import MessageSpec

class SimpleSubscribingNode(fel.NodeLifecycle):
    def __init__(self, topic):
        super().__init__()
        self.topic = topic
        self.subscriber = fel.Subscriber()

    def on_init(self):
        print("SimpleSubscribingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimpleSubscribingNode.on_did_create()")
        self.node_info = node_info
        self.request_subscribe()

    def on_error(self, status):
        print("SimpleSubscribingNode.on_error()")
        fel.log(fel.ERROR, status.error_message())

    def request_subscribe(self):
        settings = fel.Settings()
        settings.buffer_size = fel.Bytes.from_bytes(512)

        self.subscriber.request_subscribe(self.node_info, self.topic,
                                          ChannelDef.TCP | ChannelDef.UDP, MessageSpec,
                                          self.on_message, self.on_subscription_error,
                                          settings, self.on_request_subscribe)

        # fel.MasterProxy.post_delayed_task(
        #         self.request_unsubscribe, fel.from_seconds(10))

    def on_message(self, message):
        print("SimpleSubscribingNode.on_message()")
        print("message : {}".format(message))

    def on_subscription_error(self, status):
        print("SimpleSubscribingNode.on_subscription_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_request_subscribe(self, status):
        print("SimpleSubscribingNode.on_request_subscribe()")
        fel.log(fel.ERROR, status.error_message())

    def request_unsubscribe(self):
        self.subscriber.request_unsubscribe(
            self.node_info, self.topic, self.on_request_unsubscribe)

    def on_request_unsubscribe(self, status):
        print("SimpleSubscribingNode.on_request_unsubscribe()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
