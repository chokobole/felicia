# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from concurrent import futures

import grpc

import felicia_py as fel


class Server(fel.rpc._Server):
    def __init__(self, max_workers):
        super().__init__()
        self.server = grpc.server(
            futures.ThreadPoolExecutor(max_workers=max_workers))
        self.init(self.ConfigureServerAddress())

    def init(self, address):
        self.server.add_insecure_port(address)

    def start(self):
        self.server.start()
        return fel.Status.OK()

    def shutdown(self):
        self.server.stop(True)
        return fel.Status.OK()

    def get_service_type_name(self):
        raise NotImplementedError('get_service_type_name not implemented.')
