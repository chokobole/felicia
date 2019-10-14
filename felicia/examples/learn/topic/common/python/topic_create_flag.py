from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.examples.learn.common.python.node_create_flag import NodeCreateFlag
import felicia_py.command_line_interface as cli


class TopicCreateFlag(NodeCreateFlag):
    def __init__(self):
        super().__init__()
        self.is_publshing_node_flag = cli.BoolFlagBuilder().set_short_name("-p").set_help(
            "create publishing node, if set, default: false(create subscribing node)").build()
        self.use_ssl_flag = cli.BoolFlagBuilder().set_long_name(
            "--use_ssl").set_help("use communication via ssl connection, default: false").build()
        self.topic_flag = cli.StringFlagBuilder().set_short_name("-t").set_long_name(
            "--topic").set_help("name for topic, it's required to be set").build()
        self.channel_type_flag = cli.StringChoicesFlagBuilder(
            ChannelDef.Type.Name(ChannelDef.CHANNEL_TYPE_TCP),
            [
                ChannelDef.Type.Name(ChannelDef.CHANNEL_TYPE_TCP),
                ChannelDef.Type.Name(ChannelDef.CHANNEL_TYPE_UDP),
                ChannelDef.Type.Name(ChannelDef.CHANNEL_TYPE_UDS),
                ChannelDef.Type.Name(ChannelDef.CHANNEL_TYPE_SHM),
            ]).set_short_name(
            "-c").set_long_name("--channel_type").set_help("protocol to deliver message, it only works for publishing node").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        return self.check_if_flag_was_set(self.topic_flag)
