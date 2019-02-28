import argparse

class CommonOptionParser(argparse.ArgumentParser):
    def __init__(self):
        """Constructor."""
        super().__init__()
        self.add_argument('--cpu', type=str)
        self.add_argument('--apple_platform_type', type=str)
        self.add_argument('-c', '--compilation_mode', type=str)
        self.add_argument('--copt', type=str)
        self.add_argument('--cxxopt', type=str)
        self.add_argument('--conlyopt', type=str)

class BuildOptionParser(CommonOptionParser):
    def __init__(self):
        """Constructor."""
        super().__init__()


class TestOptionParser(CommonOptionParser):
    def __init__(self):
        """Constructor."""
        super().__init__()
        self.add_argument('--test_tag_filters', type=str)


def option_to_str(option):
    option_dict = vars(option)
    should_have_muliple_kv_pair = ['copt', 'cxxopt', 'conlyopt']
    option_list = []
    for k, v in option_dict.items():
        if v is None:
            continue
        if k in should_have_muliple_kv_pair:
            for o in v.split():
                option_list.append('--{} {}'.format(k, o))
        else:
            option_list.append('--{} {}'.format(k, v))
    return ' '.join(option_list)