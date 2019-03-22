import sys

import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from node_create_flag import NodeCreateFlag
from simple_publishing_node import SimplePublishigNode
from simple_subscribing_node import SimpleSubscribingNode


def main():
    delegate = NodeCreateFlag()
    parser = cli.FlagParser()
    parser.set_program_name('node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    fel.MasterProxy.start()

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value()

    if delegate.is_publshing_node_flag.value():
        fel.MasterProxy.request_register_node(
            SimplePublishigNode, node_info, delegate.topic_flag.value(), delegate.channel_type_flag.value())
    else:
        fel.MasterProxy.request_register_node(
            SimpleSubscribingNode, node_info, delegate.topic_flag.value())

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
