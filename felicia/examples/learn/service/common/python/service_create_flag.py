from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.examples.learn.common.python.node_create_flag import NodeCreateFlag
import felicia_py.command_line_interface as cli


class ServiceCreateFlag(NodeCreateFlag):
    def __init__(self):
        super().__init__()
        self.is_server_flag = cli.BoolFlagBuilder().set_long_name("--server").set_help(
            "create server node, if set, default: false(create client node)").build()
        self.service_flag = cli.StringFlagBuilder().set_short_name("-s").set_long_name("--service").set_long_name(
            "--service").set_help("name for service, it's required to be set").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        return self.check_if_flag_was_set(self.service_flag)
