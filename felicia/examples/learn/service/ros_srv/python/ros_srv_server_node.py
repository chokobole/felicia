import felicia_py as fel
from felicia.python.rpc.server import Server

from felicia.examples.learn.service.ros_srv._SimpleService import SimpleService


class RosSrvSimpleService(fel.rpc.RosService):
    def handle(self, request, response, callback):
        a = request.a
        b = request.b

        response.sum = a + b
        callback(fel.Status.OK())


class RosSrvServerNode(fel.NodeLifecycle):
    def __init__(self, simple_service_flag):
        super().__init__()
        self.service = simple_service_flag.service_flag.value
        ros_srv_simple_service = RosSrvSimpleService(SimpleService)
        self.server = fel.communication.RosServiceServer(
            ros_srv_simple_service)

    def on_did_create(self, node_info):
        self.node_info = node_info
        self.request_register()

    def request_register(self):
        self.server.request_register(
            self.node_info, self.service)

    def request_unregister(self):
        self.server.request_unregister(
            self.node_info, self.service)
