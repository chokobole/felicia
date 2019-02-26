import argparse
import os
import platform
import sys

sys.path.append(os.path.abspath(os.path.dirname(__file__)))
from lib.bazel import Bazel, QueryCommandOperand, QueryCommandExpr, Package, Target
from lib.bazel import BuildOptions, TestOptions, build_options_from_str, test_options_from_str
from lib.docker import Docker, ExecContainerOptions, ListContainerOptions, RunContainerOptions
from lib.util import is_darwin, is_linux, is_windows

class FeliciaDocker(Docker):
    def __init__(self):
        """Constructor."""
        super().__init__()
        self.image = 'felicia'
        self.user = 'felicia'
        self.name = 'felicia'

    def has_named_container(self, name):
        """Check whether there are containers running with argument name."""
        options = ListContainerOptions(format='{{.Names}}', all=True)
        names = self.list_container(options)
        return name in names.split()

    def exec_with_command(self, cmd):
        """Exececute command on docker instance."""
        if self.has_named_container(self.name):
            self.stop_container(None, self.name)
            self.remove_container(None, self.name)
        run_options = RunContainerOptions(user=self.user, name=self.name, interactive=True,
                                          tty=True, detach=True)
        self.run_container(run_options, self.image, ['/bin/bash'])
        exec_options = ExecContainerOptions(user=self.user)
        self.exec_container(exec_options, self.name, ['/bin/bash', '-c', cmd])
        self.print_success('{} on docker'.format(cmd))

class FeliciaBazel(Bazel):
    def __init__(self):
        """Constructor."""
        super().__init__()
        self.package = Package('felicia')
        self.all_package = Package('felicia', True)
        if is_darwin():
            # TODO(chokobole): For my laptop, which is Macbook Pro (Retina, 15-inch, Mid 2015)
            # platform.processor() returned i386 but bazel's cpu configuraiton only allows darwin
            # or darwin_x86_64.
            # cpu = 'darwin_{}'.format(platform.mac_ver()[-1])
            # But if |platfrom| is set to "darwin_x86_64" using above expression,
            # an error happens like below.
            # external/com_github_cares_cares/ares_process.c:198:10:
            # error: use of undeclared identifier 'MSG_NOSIGNAL'
            # So forcely I set |platform| to "darwin".
            self.platform_options = BuildOptions(cpu='darwin', apple_platform_type='macos')
        elif is_linux():
            self.platform_options = BuildOptions(cpu='k8')
        elif is_windows():
            if '64' in platform.architecture()[0]:
                self.platform_options = BuildOptions(cpu='x64_windows')


    def update_options(self, options, constructor):
        """Update options like platform options, such as cpu."""
        new_options = options._asdict()
        if new_options['cpu'] is None:
            new_options['cpu'] = self.platform_options.cpu
        if is_darwin():
            if new_options['apple_platform_type'] is None:
                new_options['apple_platform_type'] = self.platform_options.apple_platform_type
        if new_options['cxxopt'] is None:
            new_options['cxxopt'] = '-D_GLIBCXX_USE_CXX11_ABI=0'
        elif not '-D_GLIBCXX_USE_CXX11_ABI=0' in new_options['cxxopt']:
            new_options['cxxopt'] += ' -D_GLIBCXX_USE_CXX11_ABI=0'

        return constructor(*tuple(new_options.values()))

    def build_cc_all(self, options):
        """Build all c++ libraries or binaries."""
        query_cmd = QueryCommandOperand.kind(label='cc_*(library|binary)',
                                             target = self.all_package)
        targets = self.query(None, query_cmd)
        build_options = self.update_options(options, BuildOptions)
        for target in targets.split():
            target = Target.from_str(target)
            self.build(build_options, target)
        self.print_success('build_cc_all')

    def test_cc_all(self, options):
        """Test all c++ tests."""
        query_cmd = QueryCommandOperand.kind(label='cc_*test', target=self.all_package)
        if options.test_tag_filters is not None:
            tag_filters = options.test_tag_filters.split(',')
            query_cmd = QueryCommandExpr(query_cmd)
            for tag_filter in tag_filters:
                if tag_filter.startswith('-'):
                    q = QueryCommandOperand.attr(key='tags', value=tag_filter[1:],
                                                 target=self.all_package)
                    query_cmd.add_except(q)
                else:
                    q = QueryCommandOperand.attr(key='tags', value=tag_filter,
                                                 target=self.all_package)
                    query_cmd.add_intersect(q)

        targets = self.query(None, query_cmd)
        test_options = self.update_options(options, TestOptions)
        for target in targets.split():
            target = Target.from_str(target)
            self.test(test_options, target)
        self.print_success('test_cc_all')

def rebuild_command(args):
    """ Rebuild command from argument option to argument string.
        This is used to re-run with docker container.
    """
    cmd = ['python3', 'scripts/felicia.py']
    if args.build_all:
        cmd.append('--build_all')
    if args.test_all:
        cmd.append('--test_all')
    if args.options:
        cmd.append('--options "{}"'.format(args.options))
    return ' '.join(cmd)

def main():
    """This script is a utility to build or test."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--build_all", action="store_true", help="Run build all")
    parser.add_argument("--test_all", action="store_true", help="Run test all")
    parser.add_argument("--with_docker", action="store_true", help="Flag to run with docker")
    parser.add_argument("--options", type=str, default="", help="Options to pass")

    args = parser.parse_args()
    if args.with_docker:
        args.with_docker = None
        docker = FeliciaDocker()
        docker.exec_with_command(rebuild_command(args))
        return
    else:
        bazel = FeliciaBazel()
        if args.build_all:
            options = build_options_from_str(args.options)
            bazel.build_cc_all(options)
        if args.test_all:
            options = test_options_from_str(args.options)
            bazel.test_cc_all(options)

if __name__ == "__main__":
    main()
