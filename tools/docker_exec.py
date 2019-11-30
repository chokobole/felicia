# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import subprocess
import sys

import docker

from lib.text_style import TextStyle


class FeliciaDocker():
    def __init__(self):
        """Constructor."""
        super().__init__()
        self.image = 'felicia'
        self.user = 'felicia'
        self.name = 'felicia'
        self.client = docker.from_env()

    def exec_with_command(self, cmd):
        """Exececute command on docker instance."""
        containers = self.client.containers.list(
            all=True, filters={"name": self.name})
        if len(containers) > 0:
            containers[0].stop()
            containers[0].remove()
        container = self.client.containers.run(
            self.image, command=['/bin/bash'], name=self.name, user=self.user,
            tty=True, detach=True)
        # TODO: Either of 2 belows are not working on TRAVIS.
        # ret = constainer.exec_run(cmd=['/bin/bash', '-c', cmd], user=self.user)
        # => Too long to emit output
        # ret = constainer.exec_run(cmd=['/bin/bash', '-c', cmd], user=self.user, stream=True)
        # => No way to check return code
        result = subprocess.run(['docker', 'container', 'exec', '--user', self.user, self.name,
                                 '/bin/bash', '-c', cmd], stdout=subprocess.PIPE, universal_newlines=True)
        result.check_returncode()


def main():
    """This script is a utility to build or test with docker."""
    docker = FeliciaDocker()
    docker.exec_with_command(' '.join(sys.argv[1:]))


if __name__ == "__main__":
    main()
