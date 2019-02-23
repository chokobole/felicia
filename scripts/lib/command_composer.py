import os
import subprocess
import sys

from lib.print_util import PrintUtil
from lib.util import is_windows


class CommandComposer(object):
    def __init__(self, prefix):
        self.prefix = prefix

    def compose(self, action, options=None, target=None, remainder=None):
        """
        Available command
        - self.prefix action [options]
        - self.prefix action [options] target
        - self.prefix action [options] target [remainder]
        """
        cmd = [self.prefix]
        if isinstance(action, list):
            cmd.extend(action)
        else:
            assert isinstance(action, str)
            cmd.append(action)
        if options is not None:
            assert isinstance(options, str)
            cmd.extend(options.split())
        if target is not None:
            assert isinstance(target, str)
            cmd.append(target)
            if remainder is not None:
                cmd.extend(remainder)

        return cmd

    def run_and_check_returncode(self, cmd):
        PrintUtil.print_yellow('[RUN] {}'.format(" ".join(cmd)))
        result = subprocess.run(cmd, stdout=subprocess.PIPE, universal_newlines=True)
        try:
            result.check_returncode()
        except subprocess.CalledProcessError:
            if is_windows() and 'bazel' in cmd[0]:
                # Bazel on Windows error printed in stdout not stderr
                print(result.stdout)
            self.print_fail(' '.join(cmd))
            sys.exit(1)
        return result.stdout

    @staticmethod
    def print_success(content):
        PrintUtil.print_green('[SUCCESS] {}'.format(content))

    @staticmethod
    def print_fail(content):
        PrintUtil.print_red('[FAIL] {}'.format(content))