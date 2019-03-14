from felicia.python.core.node.node_lifecycle import NodeLifecycle
from felicia.core.channel.channel_pb2 import ChannelDef


class SimplePublishigNode(NodeLifecycle):
    def __init__(self, topic, channel_type):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        if channel_type == 'TCP':
            self.channel_def.type = ChannelDef.TCP
        elif channel_type == 'UDP':
            self.channel_def.type = ChannelDef.UDP

    def on_init(self):
        print("SimpePublishingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimplePublishingNode.on_did_create()")
        self.node_info = node_info

    def on_error(self, status):
        print("SimplePublishingNode.on_error()")