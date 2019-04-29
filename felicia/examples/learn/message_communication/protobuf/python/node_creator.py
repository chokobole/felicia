import sys
import platform

if platform.system() == 'Windows':
    # When you run py_binary using bazel, it constructs output layout like below
    # runfiles
    # | __init__.py
    # | - felicia
    #     | __init__.py
    #     | - felicia
    #         | - core
    #         | - examples
    # | - com_google_protobuf
    # | - six_archive
    #
    # On Windows, because PYTHONPATH contains [..., .../runfiles, .../runfiles/felicia]
    # so it is confused when python tries to import felicia, because in layout order,
    # the parent felicia will be imported first.
    for i in range(len(sys.path)):
        if sys.path[i].endswith('runfiles'):
            del sys.path[i]
            break

import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from felicia.examples.learn.message_communication.common.python.node_create_flag import NodeCreateFlag
from simple_publishing_node import SimplePublishigNode
from simple_subscribing_node import SimpleSubscribingNode


def main():
    fel.felicia_init()

    delegate = NodeCreateFlag()
    parser = cli.FlagParser()
    parser.set_program_name('node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.TextStyle.red("Error:"), s), file=sys.stderr)
        sys.exit(1)

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
