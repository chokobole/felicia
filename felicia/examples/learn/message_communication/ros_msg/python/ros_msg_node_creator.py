import sys

import import_order_resolver  # Should be before import felicia_py
import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from felicia.examples.learn.message_communication.common.python.node_create_flag import NodeCreateFlag
from ros_msg_publishing_node import RosMsgPublishingNode
from ros_msg_subscribing_node import RosMsgSubscribingNode


def main():
    fel.felicia_init()

    delegate = NodeCreateFlag()
    parser = cli.FlagParser()
    parser.set_program_name('ros_msg_node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value

    if delegate.is_publshing_node_flag.value:
        fel.MasterProxy.request_register_node(
            RosMsgPublishingNode, node_info, delegate)
    else:
        fel.MasterProxy.request_register_node(
            RosMsgSubscribingNode, node_info, delegate)

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
