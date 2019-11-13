# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

RED = "\033[0;31m"
BLUE = "\033[34m"
GREEN = "\033[32m"
YELLOW = "\033[93m"
BOLD = "\033[1m"
NONE = "\033[0m"

def styled(style, msg):
    return "%s%s%s" % (style, msg, NONE)

def red(msg):
    return styled(RED, msg)

def blue(msg):
    return styled(BLUE, msg)

def green(msg):
    return styled(GREEN, msg)

def yellow(msg):
    return styled(YELLOW, msg)

def bold(msg):
    return styled(BOLD, msg)
