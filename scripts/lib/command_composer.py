import os
import subprocess
import sys

from lib.print_util import PrintUtil

class CommandComposer(object):
    def __init__(self, prefix):
        self.prefix = prefix

    def compose(self, action, options=None, what=None, remainder=None):
        """
        Available command
        - self.prefix action [options]
        - self.prefix action [options] what
        - self.prefix action [options] what [remainder]
        """
        cmd = [self.prefix]
        if type(action) is list:
            cmd.extend(action)
        else:
            assert type(action) is str
            cmd.append(action)
        if options is not None:
            assert type(options) is str
            cmd.extend(options.split())
        if what is not None:
            assert type(what) is str
            cmd.append(what)
            if remainder is not None:
                cmd.extend(remainder)

        return cmd

    def run_and_check_returncode(self, cmd, env={}):
        PrintUtil.print_yellow('[RUN] {}'.format(' '.join(cmd)))
        env_ = os.environ
        env_.update(env)
        result = subprocess.run(cmd, stdout=subprocess.PIPE, universal_newlines=True, env=env_)
        try:
            result.check_returncode()
        except subprocess.CalledProcessError as e:
            self.print_fail(' '.join(cmd))
            sys.exit(1)
        return result.stdout

    def print_success(self, content):
        PrintUtil.print_green('[SUCCESS] {}'.format(content))

    def print_fail(self, content):
        PrintUtil.print_red('[FAIL] {}'.format(content))