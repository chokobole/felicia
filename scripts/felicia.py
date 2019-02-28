import argparse
import copy
import os
import platform
import sys

sys.path.append(os.path.abspath(os.path.dirname(__file__)))
from lib.bazel import Bazel, QueryCommandOperand, QueryCommandExpr, Package, Target
from lib.bazel_option_parser import BuildOptionParser, TestOptionParser
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
            self.platform_options = argparse.Namespace(cpu='darwin', apple_platform_type='macos')
        elif is_linux():
            self.platform_options = argparse.Namespace(cpu='k8')
        elif is_windows():
            if '64' in platform.architecture()[0]:
                self.platform_options = argparse.Namespace(cpu='x64_windows')

    def define(self, key, value = None):
        """Define CFLAGS or CXXFLAGS depending on platform."""
        if value is None:
            flag = key
        else:
            flag = '{}={}'.format(key, value)

        if is_windows():
            return '/D{}'.format(flag)
        else:
            return '-D{}'.format(flag)

    def _update_options(self, options, key, value):
        if getattr(options, key) is None:
            setattr(options, key, value)
        else:
            setattr(options, key, ' {}'.format(value))

    def update_options(self, options):
        """Update options like platform options, such as cpu."""
        new_options = copy.copy(options)
        if new_options.cpu is None:
            new_options.cpu = self.platform_options.cpu
        if is_darwin():
            if new_options.apple_platform_type is None:
                new_options.apple_platform_type = self.platform_options.apple_platform_type
        # TODO(chokobole): Detect CC and apply below only when using g++
        cxxopt = [self.define('_GLIBCXX_USE_CXX11_ABI', '0')]
        self._update_options(new_options, 'cxxopt', ' '.join(cxxopt))
        if is_windows():
            copt = [
                self.define('_UNICODE'),
                self.define('UNICODE'),
                self.define('_WIN32_WINNT', '0x0A00'),
                self.define('WINVER', '0x0A00'),
                self.define('WIN32_LEAN_AND_MEAN'),
                self.define('_WINDOWS'),
                self.define('WIN32'),
            ]
            self._update_options(new_options, 'copt', ' '.join(copt))
        return new_options

    def build_cc(self, target, options):
        """Build c++ target."""
        build_options = self.update_options(options)
        if target == 'all':
            query_cmd = QueryCommandOperand.kind(label='cc_*(library|binary)',
                                                 target = self.all_package)
            targets = self.query(None, query_cmd)
            for target in targets.split():
                target = Target.from_str(target)
                self.build(build_options, target)
        else:
            target = Target.from_str(target)
            self.build(build_options, target)
        self.print_success('build_cc')

    def test_cc(self, target, options):
        """Test c++ target."""
        test_options = self.update_options(options)
        if target == 'all':
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
            for target in targets.split():
                target = Target.from_str(target)
                self.test(test_options, target)
        else:
            target = Target.from_str(target)
            self.test(test_options, target)
        self.print_success('test_cc')

    def run_cc(self, target, options):
        """Run c++ target."""
        build_options = self.update_options(options)
        target = Target.from_str(target)
        self.run(build_options, target)
        self.print_success('run_cc')


def main():
    """This script is a utility to build or test."""
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='action', help='sub-command help')
    parser_build = subparsers.add_parser('build', help='Run build')
    parser_build.add_argument('target', type=str, help='Build specific target, if you type all, then it is same with //felicia/...')
    parser_build.add_argument('--with_test', action="store_true", help='Test on target, too.')
    parser_test = subparsers.add_parser('test', help='Run build')
    parser_test.add_argument('target', type=str, help='Test specific target, if you type all, then it is same with //felicia/...')
    parser_build = subparsers.add_parser('run', help='Run run')
    parser_build.add_argument('target', type=str, help='Run specific target')
    parser.add_argument("--with_docker", action="store_true", help="Flag to run with docker")
    parser.add_argument("--options", type=str, default="", help="Options to pass to bazel")

    args = parser.parse_args()
    if args.with_docker:
        docker = FeliciaDocker()
        sys.argv.remove('--with_docker')
        sys.argv.insert(0, 'python3')
        idx = sys.argv.index('--options')
        if idx > 0:
            sys.argv[idx + 1] = '"{}"'.format(sys.argv[idx + 1])
        docker.exec_with_command(' '.join(sys.argv))
        return
    else:
        bazel = FeliciaBazel()
        if args.action == 'build':
            parser = BuildOptionParser()
            options = parser.parse_args(args.options.split())
            bazel.build_cc(args.target, options)
            if args.with_test:
                if args.target != 'all':
                    print("Do you mean all?")
                    sys.exit(1)
                else:
                    parser = TestOptionParser()
                    options = parser.parse_args(args.options.split())
                    bazel.test_cc(args.target, options)
        elif args.action == 'test':
            parser = TestOptionParser()
            options = parser.parse_args(args.options.split())
            bazel.test_cc(args.target, options)
        elif args.action == 'run':
            parser = BuildOptionParser()
            options = parser.parse_args(args.options.split())
            bazel.run_cc(args.target, options)

if __name__ == "__main__":
    main()
