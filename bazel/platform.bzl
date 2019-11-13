# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

def is_windows(repository_ctx):
    """Returns true if the host operating system is windows."""
    os_name = repository_ctx.os.name.lower()
    return os_name.find("windows") != -1

def is_linux(repository_ctx):
    """Returns true if the host operating system is linux."""
    os_name = repository_ctx.os.name.lower()
    return os_name.find("linux") != -1

def is_unix(repository_ctx):
    """Returns true if the host operating system is unix."""
    return not is_windows(repository_ctx)

def is_x64(repository_ctx):
    result = repository_ctx.execute(["uname", "-m"])
    return result.stdout.strip() == "x86_64"
