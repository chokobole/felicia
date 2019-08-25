from felicia.core.protobuf.ui_pb2 import PixelFormat, PIXEL_FORMAT_BGR, PIXEL_FORMAT_BGRA, PIXEL_FORMAT_RGB, PIXEL_FORMAT_RGBA
from felicia.examples.learn.message_communication.common.python.node_create_flag import NodeCreateFlag
import felicia_py.command_line_interface as cli
import felicia_py as fel


class CameraFlag(NodeCreateFlag):
    def __init__(self, default_width=640, default_height=480, default_fps=30, default_pixel_format=PIXEL_FORMAT_BGR):
        super().__init__()
        self.channel_type_flag.release()
        self.channel_type_flag = None

        self.device_list_flag = cli.BoolFlagBuilder().set_short_name("-l").set_long_name("--device_list").set_help(
            "whether it shows device list, If you don't know which device index to pass, then try this option!").build()
        self.device_index_flag = cli.IntFlagBuilder().set_short_name("-i").set_long_name("--device_index").set_help(
            "device index for camera").build()
        self.width_flag = cli.IntDefaultFlagBuilder(default_width).set_short_name("-w").set_long_name("--width").set_help(
            "camera frame width (default: {})".format(default_width)).build()
        self.height_flag = cli.IntDefaultFlagBuilder(default_height).set_short_name("-h").set_long_name("--height").set_help(
            "camera frame height (default: {})".format(default_height)).build()
        self.fps_flag = cli.FloatDefaultFlagBuilder(default_fps).set_short_name("-f").set_long_name("--fps").set_help(
            "frame per second (default: {})".format(default_fps)).build()

        kDefaultPixelFormats = [
            PIXEL_FORMAT_BGR, PIXEL_FORMAT_BGRA, PIXEL_FORMAT_RGB, PIXEL_FORMAT_RGBA]
        final_pixel_format = PIXEL_FORMAT_BGR
        for pixel_format in kDefaultPixelFormats:
            if pixel_format == default_pixel_format:
                final_pixel_format = pixel_format
                break

        self.pixel_format_flag = cli.StringChoicesFlagBuilder(
            PixelFormat.Name(final_pixel_format),
            [
                PixelFormat.Name(PIXEL_FORMAT_BGR),
                PixelFormat.Name(PIXEL_FORMAT_BGRA),
                PixelFormat.Name(PIXEL_FORMAT_RGB),
                PixelFormat.Name(PIXEL_FORMAT_RGBA)
            ]).set_long_name("--pixel_format").set_help("pixel_format (default: {})".format(final_pixel_format)).build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        if self.check_if_device_list_flag_was_set():
            return True

        if not super().validate():
            return False

        return self.check_if_camrea_flags_valid(self.is_publshing_node_flag.value)

    def check_if_device_list_flag_was_set(self):
        if self.device_list_flag.is_set():
            print("device_list is on, it just shows a list of camera devices. If you pass -i(--device_index) with the -l then you can iterate the camera formats the device supports.")
            return True
        return False

    def check_if_camrea_flags_valid(self, is_publishing_node):
        if is_publishing_node:
            if (not (super().check_if_flag_positive(self.width_flag) and super().check_if_flag_positive(self.height_flag) and super().check_if_flag_positive(self.fps_flag))):
                return False
            return super().check_if_flag_was_set(self.device_index_flag)
        else:
            return super().check_if_flag_positive(self.fps_flag)
