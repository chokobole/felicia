import grpc

import felicia_py


class Client(felicia_py.rpc._Client):
    @classmethod
    def new_stub(cls, channel):
        raise NotImplementedError("new_stub not implemented")

    def Connect(self, ip_endpoint, callback):
        self.connect(
            "{}:{}".format(ip_endpoint.ip, ip_endpoint.port))
        self.stub = self.new_stub(self.channel)
        callback(felicia_py.Status.OK())

    def connect(self, address):
        self.channel = grpc.insecure_channel(address)

    def Run(self):
        return felicia_py.Status.OK()

    def Shutdown(self):
        self.stop()
        return felicia_py.Status.OK()

    def stop(self):
        self.channel.close()
