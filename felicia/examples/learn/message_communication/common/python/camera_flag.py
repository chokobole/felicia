from felicia.examples.learn.message_communication.common.python.node_create_flag import NodeCreateFlag
import felicia_py.command_line_interface as cli
import felicia_py as fel


class CameraFlag(NodeCreateFlag):
    def __init__(self):
        super().__init__()
        self.channel_type_flag.release()
        self.channel_type_flag = None

        self.device_list_flag = cli.BoolFlagBuilder().set_short_name("-l").set_long_name("--device_list").set_help(
            "whether it shows device list, If you don't know which device index to pass, then try this option!").build()
        self.device_index_flag = cli.IntFlagBuilder().set_short_name("-i").set_long_name("--device_index").set_help(
            "device index for camera").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        if self.device_list_flag.is_set():
            print("device_list is on, it just shows a list of camera devices. If you pass -i(--device_index) with the -l then you can iterate the camera formats the device supports.")
            return True

        if not super().validate():
            return False

        if self.is_publshing_node_flag.value():
            return self.device_index_flag.is_set()

        return True
