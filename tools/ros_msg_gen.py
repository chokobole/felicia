#!/usr/bin/env python

import argparse
import copy
import os
import subprocess
import sys

from lib.util import is_linux, is_windows, is_64bit

ROS_DISTRO = os.environ['ROS_DISTRO']
if is_windows():
    if is_64bit():
        ROS_ROOT = 'C:\\opt\\ros\\{}\\x64'.format(ROS_DISTRO)
    else:
        ROS_ROOT = 'C:\\opt\\ros\\{}\\x86'.format(ROS_DISTRO)
elif is_linux():
    ROS_ROOT = '/opt/ros/{}'.format(ROS_DISTRO)

ROS_LIB = os.path.join(ROS_ROOT, 'lib')
ROS_SHARE = os.path.join(ROS_ROOT, 'share')
GEN_CPP = os.path.join(ROS_LIB, 'gencpp', 'gen_cpp.py')
GEN_PY_MSG = os.path.join(ROS_LIB, 'genpy', 'genmsg_py.py')
GEN_PY_SRV = os.path.join(ROS_LIB, 'genpy', 'gensrv_py.py')

if is_windows():
    PYTHONPATH = '{}\\site-packages'.format(ROS_LIB)
    GEN_CPP += '.exe'
    GEN_PY_MSG += '.exe'
    GEN_PY_SRV += '.exe'
elif is_linux():
    PYTHONPATH = '{}/python2.7/dist-packages'.format(ROS_LIB)

def main(argv=None):
    if argv is None:
        argv = sys.argv

    parser = argparse.ArgumentParser(
        usage='ros_msg_gen.py [cpp|py_msg|py_srv] [options to delegate]')
    parser.add_argument('method', choices=[
                        'cpp', 'py_msg', 'py_srv'], help='method to generate ros message')
    parser.add_argument('--native_deps', dest='native_deps', default=[],
                        help='native dependencies for messages',
                        action='append')
    args, extras = parser.parse_known_args(argv[1:])

    cmd = []
    if args.method == 'cpp':
        cmd.append(GEN_CPP)
    elif args.method == 'py_msg':
        cmd.append(GEN_PY_MSG)
    elif args.method == 'py_srv':
        cmd.append(GEN_PY_SRV)
    else:
        return 1

    cmd.extend(extras)
    for native_dep in args.native_deps:
        cmd.extend(
            ['-I', '{}:{}/{}/msg'.format(native_dep, ROS_SHARE, native_dep)])
    result = subprocess.run(cmd, env={"PYTHONPATH": PYTHONPATH})
    return result.returncode


if __name__ == '__main__':
    sys.exit(main())
