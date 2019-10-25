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

    def start(self):
        self.server.start()
        return felicia_py.Status.OK()

    def shutdown(self):
        self.server.stop(grace)
        return felicia_py.Status.OK()

    def get_service_type_name(self):
        raise NotImplementedError('service_type not implemented.')
