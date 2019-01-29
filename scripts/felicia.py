import argparse
import os
import sys

sys.path.append(os.path.abspath(os.path.dirname(__file__)))
from lib.bazel import Bazel
from lib.docker import Docker

def rebuild_command(args):
    cmd = ['python3', 'scripts/felicia.py']
    if args.build_all:
        cmd.append('--build_all')
    if args.test_all:
        cmd.append('--test_all')
    if args.options:
        cmd.append('--options {}'.format(args.option))
    return ' '.join(cmd)

def main(args):
    bazel = Bazel()
    docker = Docker()
    if args.with_docker:
        args.with_docker = None
        docker.exec_with_command('--user {}'.format('felicia'), rebuild_command(args), 'felicia')
        return
    if args.build_all:
        bazel.build_cc_all('felicia', args.options)
    if args.test_all:
        bazel.test_cc_all('felicia', args.options)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--build_all", action="store_true", help="Run build all")
    parser.add_argument("--test_all", action="store_true", help="Run test all")
    parser.add_argument("--options", type=str, default='', help="Options to be passed")
    parser.add_argument("--with_docker", action="store_true", help="Flag to run with docker")

    args = parser.parse_args()
    main(args)