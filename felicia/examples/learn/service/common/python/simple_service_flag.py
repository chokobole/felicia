import sys

from felicia.examples.learn.service.common.python.service_create_flag import ServiceCreateFlag
import felicia_py.command_line_interface as cli
import felicia_py as fel


class SimpleServiceFlag(ServiceCreateFlag):
    def __init__(self):
        super().__init__()
        self.a_flag = cli.IntFlagBuilder().set_short_name("-a").set_help(
            "number for a").build()
        self.b_flag = cli.IntFlagBuilder().set_short_name("-b").set_help(
            "number for b").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        if self.is_server_flag.value:
            if self.a_flag.is_set() or self.b_flag.is_set():
                print("{} Either a or b was set. but if it's server, it will be ignored.".format(
                    cli.RED_ERROR), file=sys.stderr)

        return self.check_if_flag_was_set(self.service_flag)
