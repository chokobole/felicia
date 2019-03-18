import sys

from felicia.core.master.master_data_pb2 import NodeInfo
from felicia.examples.learn.message_communication.python.node_create_flag import NodeCreateFlag
from felicia.examples.learn.message_communication.python.simple_publishing_node import SimplePublishigNode
from felicia.examples.learn.message_communication.python.simple_subscribing_node import SimpleSubscribingNode
from felicia.python.core.util.command_line_interface import flag
from felicia.python.core.util.command_line_interface.flag_parser_delegate import FlagParserDelegate
from felicia.python.core.util.command_line_interface.text_style import TextStyle
from felicia.python.master_proxy import MasterProxy


def main():
    delegate = NodeCreateFlag()
    parser = flag.FlagParser()
    parser.set_program_name('node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    MasterProxy.start()

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value()

    if delegate.is_publshing_node_flag.value():
        MasterProxy.request_register_node(
            SimplePublishigNode, node_info, delegate.topic_flag.value(), delegate.channel_type_flag.value())
    else:
        MasterProxy.request_register_node(
            SimpleSubscribingNode, node_info, delegate.topic_flag.value())

    MasterProxy.run()


if __name__ == '__main__':
    main()
