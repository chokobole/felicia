from functools import partial

import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.master_data_pb2 import ServiceInfo
from felicia.python.rpc.client import Client

from felicia.examples.learn.service.ros_srv._SimpleService import SimpleService, SimpleServiceRequest, SimpleServiceResponse


class RosSrvClientNode(fel.NodeLifecycle):
    def __init__(self, simple_service_flag):
        super().__init__()
        self.simple_service_flag = simple_service_flag
        self.service = simple_service_flag.service_flag.value
        self.client = fel.communication.RosServiceClient(SimpleService)

    def on_did_create(self, node_info):
        self.node_info = node_info
        self.request_register()

    def on_connect(self, service_info_status):
        if (service_info_status == ServiceInfo.REGISTERED):
            self.request_add()
        else:
            print("disconnected...")

    def request_register(self):
        self.client.request_register(
            self.node_info, self.service, self.on_connect)

    def request_unregister(self):
        self.client.request_unregister(
            self.node_info, self.service)

    def on_request_add(self, request, response, status):
        if status.ok():
            print('{} + {} = {}'.format(str(request.a), str(request.b),
                                        cli.TextStyle.green(str(response.sum))))
        else:
            fel.log(fel.ERROR, status.error_message())

    def request_add(self):
        request = SimpleServiceRequest()
        response = SimpleServiceResponse()
        request.a = self.simple_service_flag.a_flag.value
        request.b = self.simple_service_flag.b_flag.value
        self.client.call(request, response, partial(
            self.on_request_add, request, response))
