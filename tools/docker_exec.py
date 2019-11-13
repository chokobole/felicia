# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import copy
import os
import platform
import sys

CURRENT_PATH = os.path.dirname(__file__)
PYTHON_PATH = os.path.join(CURRENT_PATH, '..', 'felicia', 'python')
sys.path.append(os.path.abspath(PYTHON_PATH))
from lib.util import is_darwin, is_linux, is_windows
from lib.docker import Docker, ExecContainerOptions, ListContainerOptions, RunContainerOptions


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


def main():
    """This script is a utility to build or test with docker."""
    docker = FeliciaDocker()
    docker.exec_with_command(' '.join(sys.argv[1:]))


if __name__ == "__main__":
    main()
