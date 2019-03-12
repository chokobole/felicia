import sys

from felicia.python.util.command_line_interface import flag
from felicia.python.util.command_line_interface.simple_flag_parser_delegate import SimpleFlagParserDelegate
from felicia.python.util.command_line_interface.text_style import TextStyle


class NodeCreateFlag(SimpleFlagParserDelegate):
    def __init__(self):
        super().__init__()
        self.is_publshing_node_flag = flag.BoolFlagBuilder().set_short_name("-p").set_help(
            "create publishing node, if set, default: false(create subscribing node)").build()
        self.name_flag = flag.StringFlagBuilder().set_long_name(
            "--name").set_help("name for node").build()
        self.topic_flag = flag.StringFlagBuilder().set_short_name("-t").set_long_name(
            "--topic").set_help("topic to publish or subscribe, it's required to be set").build()
        self.channel_type_flag = flag.StringChoicesFlagBuilder("TCP", ["TCP", "UDP"]).set_long_name(
            "--channel_type").set_help(
                "protocol to deliver message, it only works for publishing node").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        is_set = self.topic_flag.is_set()
        if not is_set:
            TextStyle.print_error("topic is not set.")
        return is_set


delegate = NodeCreateFlag()
parser = flag.FlagParser()
parser.set_program_name('node_creator')
if not parser.parse(len(sys.argv), sys.argv, delegate):
    sys.exit(1)
