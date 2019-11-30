# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from platform import system
import sys


def is_darwin():
    return system() == 'Darwin'


def is_linux():
    return system() == 'Linux'


def is_windows():
    return system() == 'Windows'


def is_64bit():
    return sys.maxsize > 2**32
