import sys

import import_order_resolver  # Should be before import felicia_py
import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from felicia.examples.learn.topic.common.python.topic_create_flag import TopicCreateFlag
from protobuf_publishing_node import ProtobufPublishingNode
from protobuf_subscribing_node import ProtobufSubscribingNode


def main():
    fel.felicia_init()

    delegate = TopicCreateFlag()
    parser = cli.FlagParser()
    parser.set_program_name('protobuf_node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value

    ssl_server_context = None
    if delegate.is_publshing_node_flag.value:
        if delegate.use_ssl_flag.value:
            cert_file_path = fel.FilePath("./felicia/examples/cert/server.crt")
            private_key_file_path = fel.FilePath(
                "./felicia/examples/cert/server.key")
            ssl_server_context = fel.channel.SSLServerContext.new_ssl_server_context(
                cert_file_path,
                private_key_file_path
            )
        fel.MasterProxy.request_register_node(
            ProtobufPublishingNode, node_info, delegate, ssl_server_context)
    else:
        fel.MasterProxy.request_register_node(
            ProtobufSubscribingNode, node_info, delegate)

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
