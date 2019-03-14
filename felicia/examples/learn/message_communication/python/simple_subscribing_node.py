from felicia.python.core.node.node_lifecycle import NodeLifecycle
from felicia.core.channel.channel_pb2 import ChannelDef


class SimpleSubscribingNode(NodeLifecycle):
    def __init__(self, topic):
        super().__init__()
        self.topic = topic

    def on_init(self):
        print("SimpleSubscribingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimpleSubscribingNode.on_did_create()")
        self.node_info = node_info

    def on_error(self, status):
        print("SimpleSubscribingNode.on_error()")