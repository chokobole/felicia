from six.moves import filter, map

import felicia.python.util.command_line_interface.flag as flag
from felicia.python.util.command_line_interface.text_style import TextStyle


class SimpleFlagParserDelegate(flag.FlagParserDelegate):
    def __init__(self):
        super().__init__()
        self._flags = None

    def _is_flag_type(self, f):
        return isinstance(f, (flag.BoolFlag, flag.IntFlag, flag.DoubleFlag, flag.StringFlag,
                              flag.BoolDefaultFlag, flag.IntDefaultFlag, flag.DoubleDefaultFlag, flag.StringDefaultFlag,
                              flag.IntRangeFlag, flag.DoubleRangeFlag, flag.StringRangeFlag,
                              flag.IntChoicesFlag, flag.DoubleChoicesFlag, flag.StringChoicesFlag))

    def _collect_flags(self):
        if self._flags is not None:
            return

        self._flags = []

        for _, v in sorted(self.__dict__.items()):
            if self._is_flag_type(v):
                self._flags.append(v)

    def Parse(self, flag_parser):
        return self.parse(flag_parser)

    def parse_optional_flags(self, flag_parser):
        self._collect_flags()
        for flag in self._flags:
            if flag.is_optional() and flag.parse(flag_parser):
                return True
        return False

    def Validate(self):
        return self.validate()

    def CollectUsages(self):
        return self.collect_usages()

    def collect_usages(self):
        self._collect_flags()
        return ['[--help]'] + list(map(lambda x: x.usage(), self._flags))

    def Description(self):
        return self.description()

    def description(self):
        return ""

    def CollectNamedHelps(self):
        return self.collect_named_helps()

    def collect_named_helps(self):
        self._collect_flags()
        positionals = filter(lambda x: x.is_positional(), self._flags)
        optionals = filter(lambda x: x.is_optional(), self._flags)
        return [
            (TextStyle.blue("Positional arguments:"), list(
                map(lambda x: x.help(), positionals)) if positionals is not None else []),
            (TextStyle.yellow("Optional arguments:"), list(
                map(lambda x: x.help(), optionals)) if optionals is not None else [])
        ]

    def __del__(self):
        self._collect_flags()
        for flag in self._flags:
            flag.release()
