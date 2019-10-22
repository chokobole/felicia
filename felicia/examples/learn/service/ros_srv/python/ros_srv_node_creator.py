import sys

import import_order_resolver  # Should be before import felicia_py
import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from ros_srv_client_node import RosSrvClientNode
from ros_srv_server_node import RosSrvServerNode
from felicia.examples.learn.service.common.python.simple_service_flag import SimpleServiceFlag


def main():
    fel.felicia_init()

    delegate = SimpleServiceFlag()
    parser = cli.FlagParser()
    parser.set_program_name('ros_srv_node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value

    if delegate.is_server_flag.value:
        fel.MasterProxy.request_register_node(
            RosSrvServerNode, node_info, delegate)
    else:
        fel.MasterProxy.request_register_node(
            RosSrvClientNode, node_info, delegate)

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
