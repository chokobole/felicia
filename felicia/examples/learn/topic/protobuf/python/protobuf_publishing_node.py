from platform import system

import felicia_py as fel
from felicia.core.protobuf.channel_pb2 import ChannelDef

from felicia.examples.learn.topic.protobuf.simple_message_pb2 import SimpleMessage


class ProtobufPublishingNode(fel.NodeLifecycle):
    def __init__(self, topic_create_flag, ssl_server_context):
        super().__init__()
        self.topic = topic_create_flag.topic_flag.value
        self.ssl_server_context = ssl_server_context
        self.channel_def_type = ChannelDef.Type.Value(
            topic_create_flag.channel_type_flag.value)
        self.publisher = fel.communication.Publisher(SimpleMessage)
        self.message_id = 0
        self.timestamper = fel.Timestamper()

    def on_init(self):
        print("ProtobufPublishingNode.on_init()")

    def on_did_create(self, node_info):
        print("ProtobufPublishingNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

        # fel.MasterProxy.post_delayed_task(
        #     self.request_unpublish, fel.TimeDelta.from_seconds(10))

    def on_error(self, status):
        print("ProtobufPublishingNode.on_error()")
        fel.log(fel.ERROR, status.error_message())

    def on_publish(self, channel_type, status):
        print("ProtobufPublishingNode.on_publish() from {}".format(
            ChannelDef.Type.Name(channel_type)))
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_request_publish(self, status):
        print("ProtobufPublishingNode.on_request_publish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())
        self.repeating_publish()

    def on_request_unpublish(self, status):
        print("ProtobufPublishingNode.on_request_unpublish()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_auth(self, credentials):
        uid = credentials.user_id
        gid = credentials.group_id
        if hasattr(credentials, "process_id"):
            pid = credentials.process_id
            print("pid: {} uid: {} gid: {}".format(pid, uid, gid))
        else:
            print("uid: {} gid: {}".format(uid, gid))
        return True

    def request_publish(self):
        settings = fel.communication.Settings()
        size = fel.Bytes.from_bytes(512)
        settings.buffer_size = size
        if self.channel_def_type == ChannelDef.CHANNEL_TYPE_TCP:
            if self.ssl_server_context is not None:
                settings.channel_settings.tcp_settings.use_ssl = True
                settings.channel_settings.tcp_settings.ssl_server_context = self.ssl_server_context
        elif self.channel_def_type == ChannelDef.CHANNEL_TYPE_UDS:
            if hasattr(settings.channel_settings, "uds_settings"):
                settings.channel_settings.uds_settings.auth_callback = self.on_auth
            else:
                raise TypeError(
                    "CHANNEL_TYPE_UDS type not support on platform: {}".format(system()))
        elif self.channel_def_type == ChannelDef.CHANNEL_TYPE_SHM:
            settings.channel_settings.shm_settings.shm_size = size

        self.publisher.request_publish(self.node_info, self.topic, self.channel_def_type,
                                       settings, self.on_request_publish)

    def request_unpublish(self):
        self.publisher.request_unpublish(self.node_info, self.topic,
                                         self.on_request_unpublish)

    def repeating_publish(self):
        self.publisher.publish(self.generate_message(), self.on_publish)

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
