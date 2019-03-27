from felicia.python.command_line_interface.flag_parser_delegate import FlagParserDelegate
import felicia_py.command_line_interface as cli


class NodeCreateFlag(FlagParserDelegate):
    def __init__(self):
        super().__init__()
        self.is_publshing_node_flag = cli.BoolFlagBuilder().set_short_name("-p").set_help(
            "create publishing node, if set, default: false(create subscribing node)").build()
        self.name_flag = cli.StringFlagBuilder().set_long_name(
            "--name").set_help("name for node").build()
        self.topic_flag = cli.StringFlagBuilder().set_short_name("-t").set_long_name(
            "--topic").set_help("topic to publish or subscribe, it's required to be set").build()
        self.channel_type_flag = cli.StringChoicesFlagBuilder("TCP", ["TCP", "UDP"]).set_long_name(
            "--channel_type").set_help(
                "protocol to deliver message, it only works for publishing node").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        is_set = self.topic_flag.is_set()
        if not is_set:
            print("{} tpoic is not set.".format(cli.TextStyle.red("Error:")))
        return is_set
