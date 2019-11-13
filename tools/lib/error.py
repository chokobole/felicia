# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


def invalud_argument(text):
    raise ValueError('Invalud Argument: {}'.format(text))


def not_reached():
    raise RuntimeError('Should not reached')
