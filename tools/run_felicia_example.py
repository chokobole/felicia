# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import fileinput
import os
import shutil
import subprocess
import tarfile

from git import Repo

from lib.util import is_darwin, is_windows

WORKING_DIR = 'felicia-examples'
INCLUDE_DIR = os.path.join(WORKING_DIR, 'include')
LIB_DIR = os.path.join(WORKING_DIR, 'lib')


def force_mkdir(path):
    """Make empty directory for the argument path."""
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path)


def run_and_check_returncode(cmd, **kwargs):
    result = subprocess.run(cmd, stdout=subprocess.PIPE,
                            universal_newlines=True, **kwargs)
    result.check_returncode()


def setup_felicia_examples():
    """Clone the git repository felicia-examples and setup."""
    if not os.path.exists(WORKING_DIR):
        Repo.clone_from(
            'https://github.com/chokobole/felicia-examples.git', WORKING_DIR)

    force_mkdir(INCLUDE_DIR)
    force_mkdir(LIB_DIR)

    if is_darwin():
        with open(os.path.join(WORKING_DIR, '.bazelrc.user'), 'w') as f:
            f.write('build --cpu darwin_x86_64\n')


def run_build_shlib():
    """Build shared library and header."""
    def run_build(cmd, target):
        cmd.append(target)
        run_and_check_returncode(cmd)
        cmd.pop()

    cmd = ['bazel', 'build', '--define', 'framework_shared_object=true']
    if is_darwin():
        run_build(cmd, '//felicia:libfelicia.dylib')
        shutil.copy('bazel-bin/felicia/libfelicia.dylib', LIB_DIR)
    elif is_windows():
        run_build(cmd, '//felicia:felicia.dll')
        shutil.copy('bazel-bin/felicia/felicia.dll', LIB_DIR)
        run_build(cmd, '//felicia:felicia_lib')
        shutil.copy('bazel-bin/felicia/felicia.lib', LIB_DIR)
    else:
        run_build(cmd, '//felicia:libfelicia.so')
        shutil.copy('bazel-bin/felicia/libfelicia.so', LIB_DIR)
    run_build(cmd, '//felicia:felicia_hdrs')
    shutil.copy('bazel-bin/felicia/felicia_hdrs.tar', INCLUDE_DIR)
    with tarfile.open(os.path.join(INCLUDE_DIR, 'felicia_hdrs.tar')) as f:
        f.extractall(INCLUDE_DIR)


def update_commit_version():
    repo = Repo(".")
    commit = repo.head.commit
    lines = fileinput.input(os.path.join(
        WORKING_DIR, 'deps.bzl'), inplace=True)
    for line in lines:
        print(line, end='')
        if line.find('name = "com_github_chokobole_felicia"') > 0:
            break
    for line in lines:
        idx = line.find('commit = ')
        if idx > 0:
            print(' ' * idx + 'commit = "{}",'.format(commit))
            break
        else:
            print(line, end='')
    for line in lines:
        print(line, end='')


def main():
    """This script is a utility to run felicia-examples."""
    setup_felicia_examples()

    run_build_shlib()

    update_commit_version()

    cmd = ['bazel', 'build', '--define',
           'use_shared_library=true', '//examples/...']
    run_and_check_returncode(cmd, cwd=WORKING_DIR)


if __name__ == "__main__":
    main()
