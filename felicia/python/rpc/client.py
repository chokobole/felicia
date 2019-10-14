import grpc

import felicia_py


class Client(felicia_py.rpc._Client):
    @classmethod
    def new_stub(cls, channel):
        raise NotImplementedError("new_stub not implemented")

    def ConnectAndRun(self, ip_endpoint):
        self.connect_and_run(
            "{}:{}".format(ip_endpoint.ip, ip_endpoint.port))
        self.stub = self.new_stub(self.channel)
        return felicia_py.Status.OK()

    def connect_and_run(self, address):
        self.channel = grpc.insecure_channel(address)

    def Shutdown(self):
        self.stop()
        return felicia_py.Status.OK()

    def stop(self):
        self.channel.close()
