# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from collections import namedtuple
from platform import system
import sys


def nametuple_with_defaults_none(name, fields):
    """Make namedtuple with defaults valua of None."""
    nt = namedtuple(name, fields)
    nt.__new__.__defaults__ = (None,) * len(nt._fields)
    return nt


def is_darwin():
    return system() == 'Darwin'


def is_linux():
    return system() == 'Linux'


def is_windows():
    return system() == 'Windows'


def is_64bit():
    return sys.maxsize > 2**32
