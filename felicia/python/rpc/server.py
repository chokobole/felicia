from concurrent import futures

import grpc

import felicia_py


class Server(felicia_py.rpc._Server):
    def __init__(self, max_workers):
        super().__init__()
        self.server = grpc.server(
            futures.ThreadPoolExecutor(max_workers=max_workers))
        self.init(self.ConfigureServerAddress())

    def init(self, address):
        self.server.add_insecure_port(address)

    def Start(self):
        self.start()
        return felicia_py.Status.OK()

    def start(self):
        self.server.start()

    def Shutdown(self):
        self.stop()
        return felicia_py.Status.OK()

    def stop(self, grace=None):
        self.server.stop(grace)

    def service_name(self):
        raise NotImplementedError('service_name not implemented.')
