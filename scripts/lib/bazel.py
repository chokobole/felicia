import os
import platform

from lib.command_composer import CommandComposer
from lib.print_util import PrintUtil

class Bazel(CommandComposer):
    def __init__(self):
        super().__init__('bazel')
        options = []
        system = platform.system()
        if system == 'Darwin':
            options.extend(['--cpu', 'darwin_{}'.format(platform.mac_ver()[-1])])
            options.extend(['--apple_platform_type', 'macos'])
            self.env = {'PATH': '{}:{}'.format(os.environ['PATH'], '{}/bin'.format(os.environ['HOME']))}
        elif system == 'Linux':
            self.env = {'PATH': '{}:{}'.format(os.environ['PATH'], '{}/bin'.format(os.environ['HOME']))}
        self.options = ' '.join(options)

    def build(self, options, label):
        cmd = self.compose('build', options, label)
        return self.run_and_check_returncode(cmd, env=self.env)

    def test(self, options, label):
        cmd = self.compose('test', options, label)
        return self.run_and_check_returncode(cmd, env=self.env)

    def query(self, options, what):
        cmd = self.compose('query', options, what)
        return self.run_and_check_returncode(cmd, env=self.env)

    def build_cc_all(self, package, options):
        options += self.options
        labels = self.query(None, 'kind(\"cc_.*(library|binary)\", //{}/...)'.format(package))
        for label in labels.split():
            self.build(options, label)
        self.print_success('build_cc_all')

    def test_cc_all(self, package, options):
        options += self.options
        labels = self.query(None, 'kind("cc_.*test", {}/...)'.format(package))
        for label in labels.split():
            self.test(options, label)
        self.print_success('test_cc_all')
