import argparse
import copy
import os
import platform
import sys

sys.path.append(os.path.abspath(os.path.dirname(__file__)))
from lib.bazel import Bazel, QueryCommandOperand, QueryCommandExpr, Package, Target
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

    def update_options(self, options):
        """Update options to load from .bazelrc."""
        if is_darwin():
            options += " --config darwin"
        elif is_linux():
            options += " --config linux"
        elif is_windows():
            options += " --config windows"
        return options

    def build_cc(self, target, options):
        """Build c++ target."""
        if target == 'all':
            query_cmd = QueryCommandOperand.kind(label='cc_*(library|binary)',
                                                 target = self.all_package)
            targets = self.query(None, query_cmd)
            for target in targets.split():
                target = Target.from_str(target)
                self.build(options, target)
        else:
            target = Target.from_str(target)
            self.build(options, target)
        self.print_success('build_cc')

    def test_cc(self, target, options):
        """Test c++ target."""
        if target == 'all':
            query_cmd = QueryCommandOperand.kind(label='cc_*test', target=self.all_package)
            if '--test_tag_filters' in options:
                options = options.split()
                idx = options.index('--test_tag_filters')
                tag_filters = options[idx + 1]
                options = options[:idx] + options[idx + 2:] # Remove test_tag_filters options
                options = ' '.join(options)
                tag_filters = tag_filters.split(',')
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
                self.test(options, target)
        else:
            target = Target.from_str(target)
            self.test(options, target)
        self.print_success('test_cc')

    def run_cc(self, target, options):
        """Run c++ target."""
        target = Target.from_str(target)
        self.run(options, target)
        self.print_success('run_cc')


def main():
    """This script is a utility to build or test."""
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='action', help='sub-command help')
    parser_build = subparsers.add_parser('build', help='Run build')
    parser_build.add_argument('target', type=str, help='Build specific target, if you type all, then it is same with //felicia/...')
    parser_build.add_argument('--with_test', action="store_true", help='Test on target, too.')
    parser_build.add_argument('--options', type=str, default="", help="Options to pass to bazel")
    parser_test = subparsers.add_parser('test', help='Run build')
    parser_test.add_argument('target', type=str, help='Test specific target, if you type all, then it is same with //felicia/...')
    parser_test.add_argument('--options', type=str, default="", help="Options to pass to bazel")
    parser_run = subparsers.add_parser('run', help='Run run')
    parser_run.add_argument('target', type=str, help='Test specific target')
    parser_run.add_argument('--options', type=str, default="", help="Options to pass to bazel")
    parser.add_argument("--with_docker", action="store_true", help="Flag to run with docker")

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
        options = bazel.update_options(args.options)
        if args.action == 'build':
            bazel.build_cc(args.target, options)
            if args.with_test:
                if args.target != 'all':
                    print("Do you mean all?")
                    sys.exit(1)
                else:
                    bazel.test_cc(args.target, options)
        elif args.action == 'test':
            bazel.test_cc(args.target, options)
        elif args.action == 'run':
            bazel.run_cc(args.target, options)

if __name__ == "__main__":
    main()
