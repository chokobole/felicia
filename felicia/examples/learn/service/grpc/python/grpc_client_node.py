import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.master_data_pb2 import ServiceInfo
from felicia.python.rpc.client import Client

import felicia.examples.learn.service.grpc.simple_service_pb2 as simple_service_pb2
import felicia.examples.learn.service.grpc.simple_service_pb2_grpc as simple_service_pb2_grpc


class SimpleClient(Client):
    @classmethod
    def new_stub(cls, channel):
        return simple_service_pb2_grpc.SimpleServiceStub(channel)

    def Add(self, request):
        return self.stub.Add(request)


class GrpcClientNode(fel.NodeLifecycle):
    def __init__(self, grpc_service_flag):
        super().__init__()
        self.grpc_service_flag = grpc_service_flag
        self.service = grpc_service_flag.service_flag.value
        self.simple_client = SimpleClient()
        self.client = fel.communication.ServiceClient(self.simple_client)

    def on_init(self):
        print("GrpcClientNode.on_init()")

    def on_did_create(self, node_info):
        print("GrpcClientNode.on_did_create()")
        self.node_info = node_info
        self.request_register()

    def on_error(self, status):
        print("GrpcClientNode.on_error()")
        fel.log(fel.ERROR, status.error_message())

    def on_request_register(self, status):
        print("GrpcClientNode.on_request_register()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_request_unregister(self, status):
        print("GrpcClientNode.on_request_unregister()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_connect(self, service_info_status):
        print("GrpcClientNode.on_connect()")
        if (service_info_status == ServiceInfo.REGISTERED):
            self.request_add()
        else:
            print("disconnected...")

    def request_register(self):
        self.client.request_register(
            self.node_info, self.service, self.on_connect, self.on_request_register)

    def request_unregister(self):
        self.client.request_unregister(
            self.node_info, self.service, self.on_request_unregister)

    def request_add(self):
        a = self.grpc_service_flag.a_flag.value
        b = self.grpc_service_flag.b_flag.value
        response = self.simple_client.Add(
            simple_service_pb2.AddRequest(a=a, b=b))
        print('{} + {} = {}'.format(a, b,
                                    cli.TextStyle.green(str(response.sum))))
