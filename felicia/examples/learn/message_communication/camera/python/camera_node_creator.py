import sys

# Should be before import felicia_py
import felicia.examples.learn.message_communication.common.python.import_order_resolver
import felicia_py as fel
from felicia.core.protobuf.master_data_pb2 import NodeInfo
import felicia_py.command_line_interface as cli

from felicia.examples.learn.message_communication.common.python.camera_flag import CameraFlag
from camera_publishing_node import CameraPublishingNode
from camera_subscribing_node import CameraSubscribingNode


def main():
    fel.felicia_init()

    delegate = CameraFlag()
    parser = cli.FlagParser()
    parser.set_program_name('camera_node_creator')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    camera_descriptors = []
    s = fel.drivers.CameraFactory.get_camera_descriptors(camera_descriptors)
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    if delegate.device_list_flag.value():
        if delegate.device_index_flag.is_set():
            camera_formats = []
            s = fel.drivers.CameraFactory.get_supported_camera_formats(
                camera_descriptors[delegate.device_index_flag.value()],
                camera_formats)
            if not s.ok():
                print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
                sys.exit(1)
            for i in range(len(camera_formats)):
                print("[{}] {}".format(i, camera_formats[i]))
        else:
            for i in range(len(camera_descriptors)):
                print("[{}] {}".format(i, camera_descriptors[i]))
        sys.exit(0)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()
    node_info.name = delegate.name_flag.value()

    if delegate.is_publshing_node_flag.value():
        fel.MasterProxy.request_register_node(
            CameraPublishingNode, node_info,
            delegate.topic_flag.value(),
            camera_descriptors[delegate.device_index_flag.value()])
    else:
        fel.MasterProxy.request_register_node(
            CameraSubscribingNode, node_info,
            delegate.topic_flag.value())

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()
