# ROS Service

In this example, you will learn how to make your own custom service and initiate communication between nodes using `ROS Service`.

Here you need three terminals, each for running the master server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build --define has_ros=true //felicia/examples/learn/service/ros_srv/cc:ros_srv_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

On the second and third shell prompts, execute the ***service client*** and ***service server***. In order to run an example node in server mode, you need to pass `-s`, a service name to serve, with an additional argument `--server` to indicate we want to make it a server. You can also pass `--name` argument to the command. Then, the master server will try to generate a node with the name unless there is an already registered node with the name.

Running a ros_srv_node_creator in ***server*** mode:
```bash
bazel-bin/felicia/examples/learn/service/ros_srv/cc/ros_srv_node_creator -s message --server
```

Running a node ros_srv_node_creator in ***client*** mode (without `--server` option):
```bash
bazel-bin/felicia/examples/learn/service/ros_srv/cc/ros_srv_node_creator -s message -a 1 -b 2
```

We don't explain the same we had in [README.md](/felicia/examples/learn/service/grpc/cc/README.md).

If you want to serve or request service through ROS, then add prefix `ros://` to service. But in this case, service should be resolved with namespace.

For service server, you have to define logic for your custom service like [ros_srv_server_node.cc](ros_srv_server_node.cc). Please note that in ROS service, unlike grpc there's always one method for one service, so you just need to define `Handle` method and it's okay.

```c++
void RosSrvSimpleService::Handle(const SimpleServiceRequest* request,
                                 SimpleServiceResponse* response,
                                 StatusOnceCallback callback) {
  int a = request->a;
  int b = request->b;

  response->sum = a + b;
  std::move(callback).Run(Status::OK());
}
```

[ros_srv_client_node.cc](ros_srv_client_node.cc) is very similar to grpc example. Here also because of ROS service characteristic above, you don't need class for elaborate whate methods in your service. You can just call rpc methods using `Call` method.

```c++
void RosSrvClientNode::RequestAdd() {
  SimpleServiceRequest* request = new SimpleServiceRequest();
  SimpleServiceResponse* response = new SimpleServiceResponse();
  request->a = simple_service_flag_.a_flag()->value();
  request->b = simple_service_flag_.b_flag()->value();
  client_->Call(
      request, response,
      base::BindOnce(&RosSrvClientNode::OnRequestAdd, base::Unretained(this),
                     base::Owned(request), base::Owned(response)));
}
```
