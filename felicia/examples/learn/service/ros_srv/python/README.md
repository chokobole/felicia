# ROS Service

In this example, you will learn how to make your own custom service and initiate communication between nodes using `ROS Service`.

Here you need three terminals, each for running the master server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build --define has_ros=true //felicia/examples/learn/service/ros_srv/python:ros_srv_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

On the second and third shell prompts, execute the ***service client*** and ***service server***. In order to run an example node in server mode, you need to pass `-s`, a service name to serve, with an additional argument `--server` to indicate we want to make it a server. You can also pass `--name` argument to the command. Then, the master server will try to generate a node with the name unless there is an already registered node with the name.

Running a ros_srv_node_creator in ***server*** mode:
```bash
bazel-bin/felicia/examples/learn/service/ros_srv/python/ros_srv_node_creator -s message --server
```

Running a node ros_srv_node_creator in ***client*** mode (without `--server` option):
```bash
bazel-bin/felicia/examples/learn/service/ros_srv/python/ros_srv_node_creator -s message -a 1 -b 2
```

We don't explain the same we had in [README.md](/felicia/examples/learn/service/grpc/python/README.md).

If you want to serve or request service through ROS, then add prefix `ros://` to service. But in this case, service should be resolved with namespace.

For service server, you have to define logic for your custom service like [ros_srv_server_node.py](ros_srv_server_node.py). Please note that in ROS service, unlike grpc there's always one method for one service, so you just need to define `Handle` method and it's okay.

```python
class RosSrvSimpleService(fel.rpc.RosService):
    def Handle(self, request, response, callback):
        a = request.a
        b = request.b

        response.sum = a + b
        callback(fel.Status.OK())
```

[ros_srv_client_node.py](ros_srv_client_node.py) is very similar to grpc example. Here also because of ROS service characteristic above, you don't need class for elaborate whate methods in your service. You can just call rpc methods using `call` method.

```python
def request_add(self):
    request = SimpleServiceRequest()
    response = SimpleServiceResponse()
    request.a = self.simple_service_flag.a_flag.value
    request.b = self.simple_service_flag.b_flag.value
    self.client.call(request, response, partial(
        self.on_request_add, request, response))
```
