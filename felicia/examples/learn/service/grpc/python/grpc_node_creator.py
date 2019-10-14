import sys

import import_order_resolver  # Should be before import felicia_py
import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from grpc_client_node import GrpcClientNode
from grpc_server_node import GrpcServerNode
from grpc_service_flag import GrpcServiceFlag


def main():
    fel.felicia_init()

    delegate = GrpcServiceFlag()
    parser = cli.FlagParser()
    parser.set_program_name('grpc_node_creator')
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
            GrpcServerNode, node_info, delegate)
    else:
        fel.MasterProxy.request_register_node(
            GrpcClientNode, node_info, delegate)

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
