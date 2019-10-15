#!/usr/bin/env python
import argparse
import copy
import os
import subprocess
import sys

if 'ROS_DISTRO' in os.environ:
    ROS_DISTRO = os.environ['ROS_DISTRO']
    ROS_ROOT = '/opt/ros/{}'.format(ROS_DISTRO)
else:
    result = subprocess.run(['which', 'roscore'], stdout=subprocess.PIPE, universal_newlines=True)
    ROS_ROOT = os.path.normpath(result.stdout + '/../../')
    ROS_DISTRO = os.path.basename(ROS_ROOT)
    del result

ROS_LIB = '{}/lib'.format(ROS_ROOT)
ROS_SHARE = '{}/share'.format(ROS_ROOT)
GEN_CPP = '{}/gencpp/gen_cpp.py'.format(ROS_LIB)
GEN_PY_MSG = '{}/genpy/genmsg_py.py'.format(ROS_LIB)
GEN_PY_SRV = '{}/genpy/gensrv_py.py'.format(ROS_LIB)

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
        cmd.extend(['-I', '{}:{}/{}/msg'.format(native_dep, ROS_SHARE, native_dep)])
    result = subprocess.run(cmd, env={"PYTHONPATH": '{}/python2.7/dist-packages'.format(ROS_LIB)})
    return result.returncode


if __name__ == '__main__':
    sys.exit(main())
