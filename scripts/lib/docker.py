from lib.command_composer import CommandComposer
from lib.util import nametuple_with_defaults_none, is_linux

ExecContainerOptions = nametuple_with_defaults_none('ExecContainerOptions', ['user'])
ListContainerOptions = nametuple_with_defaults_none('ListContainerOptions', ['format', 'all'])
RunContainerOptions = nametuple_with_defaults_none('RunContainerOptions',
                                                   ['user', 'name', 'detach', 'interactive', 'tty'])


class Docker(CommandComposer):
    def __init__(self):
        """Constructor."""
        super().__init__('docker')

    def run_and_check_returncode(self, cmd):
        """Override |run_and_check_returncode| attaching sudo privilege,
           if needed."""
        if is_linux():
            cmd.insert(0, 'sudo')
        return super().run_and_check_returncode(cmd)

    def exec_container(self, options, container, remainder):
        """Execute container."""
        if options is not None:
            opts = []
            if options.user is not None:
                opts.append('--user {}'.format(options.user))
            options = ' '.join(opts)
        cmd = self.compose(['container', 'exec'], options, container, remainder)
        return self.run_and_check_returncode(cmd)

    def list_container(self, options):
        """List container."""
        opts = []
        if options.format is not None:
            opts.append('--format {}'.format(options.format))
        if options.all is not None and options.all:
            opts.append('-a')
        cmd = self.compose(['container', 'ls'], ' '.join(opts))
        return self.run_and_check_returncode(cmd)

    def remove_container(self, options, container):
        """Remove container."""
        cmd = self.compose(['container', 'rm'], options, container)
        return self.run_and_check_returncode(cmd)

    def run_container(self, options, image, remainder):
        """Run container."""
        if options is not None:
            opts = []
            if options.user is not None:
                opts.append('--user {}'.format(options.user))
            if options.name is not None:
                opts.append('--name {}'.format(options.name))
            if options.detach is not None and options.detach:
                opts.append('-d')
            if options.interactive is not None and options.interactive:
                opts.append('-i')
            if options.tty is not None and options.tty:
                opts.append('-t')
            options = ' '.join(opts)
        cmd = self.compose(['container', 'run'], options, image, remainder)
        return self.run_and_check_returncode(cmd)

    def start_container(self, options, container):
        """Start container."""
        cmd = self.compose(['container', 'start'], options, container)
        return self.run_and_check_returncode(cmd)

    def stop_container(self, options, container):
        """Stop container."""
        cmd = self.compose(['container' ,'stop'], options, container)
        return self.run_and_check_returncode(cmd)
