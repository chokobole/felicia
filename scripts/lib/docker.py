from lib.command_composer import CommandComposer
from lib.print_util import PrintUtil

class Docker(CommandComposer):
    def __init__(self):
        super().__init__('docker')

    def exec_container(self, options, container, remainder):
        cmd = self.compose(['container', 'exec'], options, container, remainder)
        return self.run_and_check_returncode(cmd)

    def ls_container(self, options):
        cmd = self.compose(['container', 'ls'], options)
        return self.run_and_check_returncode(cmd)

    def remove_container(self, options, container):
        cmd = self.compose(['container', 'rm'], options, container)
        return self.run_and_check_returncode(cmd)

    def run_container(self, options, image, remainder):
        cmd = self.compose(['container', 'run'], options, image, remainder)
        return self.run_and_check_returncode(cmd)

    def start_container(self, options, container):
        cmd = self.compose(['container', 'start'], options, container)
        return self.run_and_check_returncode(cmd)

    def stop_container(self, options, container):
        cmd = self.compose(['container' ,'stop'], options, container)
        return self.run_and_check_returncode(cmd)

    def has_named_container(self, name):
        names = self.ls_container('--format {{.Names}} -a')
        return name in names.split()

    def exec_with_command(self, options, cmd, image, name=None):
        if name is None:
            name = image
        if options is None:
            options = ''
        if self.has_named_container(name):
            self.stop_container(None, name)
            self.remove_container(None, name)
        self.run_container(options + ' -it -d --name {}'.format(name), image, ['/bin/bash'])
        self.exec_container(options, name, ['/bin/bash', '-c', cmd])
        self.print_success('{} on docker'.format(cmd))

