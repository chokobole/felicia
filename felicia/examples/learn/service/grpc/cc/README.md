# Grpc Service

A 'Hello World' example of ***C++*** service programming.

In this example, you will learn how to make your own custom service and initiate communication between nodes.

Here you need three terminals, each for running the master server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build //felicia/examples/learn/service/grpc/cc:grpc_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

On the second and third shell prompts, execute the ***service client*** and ***service server***. In order to run an example node in server mode, you need to pass `-s`, a service name to serve, with an additional argument `--server` to indicate we want to make it a server. You can also pass `--name` argument to the command. Then, the master server will try to generate a node with the name unless there is an already registered node with the name.

Running a grpc_node_creator in ***server*** mode:
```bash
bazel-bin/felicia/examples/learn/service/grpc/cc/grpc_node_creator -s message --server
```

Running a node grpc_node_creator in ***client*** mode (without `--server` option):
```bash
bazel-bin/felicia/examples/learn/service/grpc/cc/grpc_node_creator -s message -a 1 -b 2
```

For further details, take a look at [grpc_node_creator.cc](grpc_node_creator.cc).

At the very first time, you have to start `MasterProxy`.

```c++
MasterProxy& master_proxy = MasterProxy::GetInstance();
master_proxy.Start();
```

Inside `Start()`, Do 4 things.
* Connect to grpc server.
* Start `HeartBeatSignaller`, which is responsible to make it alive by signalling heart beat to the server.
* Start `MasterNotificationWatcher`, which is responsible to watch any service source, if a node wants to request.
* Lastly if the 3 aboves are completed, then register itself to server.

If you pass `--name` to the command line, then set the name.

```c++
NodeInfo node_info;
node_info.set_name(delegate.name());
```

Now register Node. It depends on whether you pass the `--server` flag.

```c++
if (delegate.is_server_flag()->value()) {
  master_proxy.RequestRegisterNode<GrpcServerNode>(node_info, delegate);
} else {
  master_proxy.RequestRegisterNode<GrpcClientNode>(node_info, delegate);
}
```

Lastly Run MasterProxy. This will blocks until `Stop()` is called.

```c++
master_proxy.Run();
```

Now look into the [grpc_server_node.h](grpc_server_node.h). Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```c++
namespace felicia {

class GrpcServerNode: public NodeLifecycle {
 public:
  GrpcServerNode(const SimpleServiceFlag& simple_service_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(NodeInfo node_info) override;
  void OnError(Status status) override;

  ...
};

}  // namespace felicia
```

Inside `MasterProxy::RequestRegisterNode`, it tries to request grpc to register node.
Before requiest, `OnInit()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random unique name. If it succeeds to register the node, then `OnDidCreate(NodeInfo)` is called. While this process, if it happens an error, `OnError(Status)` will be called.


Then how is possibly serve services? If you want to serve a service, you have to use `ServiceServer<T>` and it is very simple to use. Very first, you have to request to the master server that we hope to serve a service.

```c++
void GrpcServerNode::RequestRegister() {
  server_.RequestRegister(node_info_, service_,
                          base::BindOnce(&GrpcServerNode::OnRequestRegister,
                                         base::Unretained(this)));
}
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though.

If request is successfully delivered to the master server, then callback `OnRequestRegister` will be called

```c++
void GrpcServerNode::OnRequestRegister(Status s) {
  std::cout << "GrpcServerNode::OnRequestRegister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}
```

Okay, now we know how to request a service server to the master server, then how can we control our logic for a service? For this, you should put 3 things.
* Fill in EnqueueRequests() methods with FEL_ENQUEUE_REQUEST(clazz, method, supports_cancel).
* Define methods with FEL_GRPC_SERVICE_METHOD_DEFINE(clazz, instance, method, supports_cancel).
* Write custom logic for your service.

```c++
// Fill in EnqueueRequests() methods with FEL_ENQUEUE_REQUEST(clazz, method, supports_cancel).
void GrpcSimpleService::EnqueueRequests() {
  FEL_ENQUEUE_REQUEST(GrpcSimpleService, Add, false);
}

// Define methods with FEL_GRPC_SERVICE_METHOD_DEFINE(clazz, instance, method, supports_cancel).
FEL_GRPC_SERVICE_METHOD_DEFINE(GrpcSimpleService, this, Add, false)

// Write custom login for your service.
void GrpcSimpleService::Add(const AddRequest* request, AddResponse* response,
                            StatusOnceCallback callback) {
  int a = request->a();
  int b = request->b();

  response->set_sum(a + b);
  std::move(callback).Run(Status::OK());
}
```

Maybe you can notice, here we have a convention to write a proto file. You have to name and every rpc method arument type and return type something like `MethodRequest` and `MethodResponse` in order to use macros used above.

```protobuf
service Service {
// You should replace Method!
rpc Method(MethodRequest) returns (MethodResponse) {}
...
}
```

To use `unregister` method, you have to do like below.

```c++
void GrpcServerNode::RequestUnregister() {
  server_.RequestUnregister(node_info_, service_,
                            base::BindOnce(&GrpcServerNode::OnRequestUnregister,
                                           base::Unretained(this)));
}
```

Same with above, if request is successfully delivered to the server, then callback
will be called, too.

```c++
void GrpcServerNode::OnRequestUnregister(Status s) {
  std::cout << "GrpcServerNode::OnRequestUnregister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}
```

[grpc_client_node.cc](grpc_client_node.cc) is very similar to above. First let's figure out how to define your client for service. It's simple you just declare and define method for your service!

* Declare methods with FEL_GRPC_CLIENT_METHOD_DECLARE(method).
* Define methods with FEL_GRPC_CLIENT_METHOD_DEFINE(clazz, method).

```c++
// grpc_client_node.h
class GrpcSimpleClient : public rpc::Client<grpc::SimpleService> {
 public:
  FEL_GRPC_CLIENT_METHOD_DECLARE(Add);
};

// grpc_client_node.cc
FEL_GRPC_CLIENT_METHOD_DEFINE(GrpcSimpleClient, Add)
```

Also unlike `service server` you have to pass one more callback, for inform you whether the `service client` is connected to the `service server`.

```c++
void GrpcClientNode::OnConnect(ServiceInfo::Status s) {
  std::cout << "GrpcClientNode::OnConnect()" << std::endl;
  if (s == ServiceInfo::REGISTERED) {
    RequestAdd();
  } else {
    std::cout << "disconnected..." << std::endl;
  }
}

void GrpcClientNode::RequestRegister() {
  client_.RequestRegister(
      node_info_, service_,
      base::BindRepeating(&GrpcClientNode::OnConnect, base::Unretained(this)),
      base::BindOnce(&GrpcClientNode::OnRequestRegister,
                     base::Unretained(this)));
}
```

If it is succesfully connected, then we have to request service! Response will be sent you back asynchronously.

```c++
void GrpcClientNode::OnRequestAdd(const AddRequest* request,
                                  AddResponse* response, Status s) {
  std::cout << "GrpcClientNode::OnRequestAdd()" << std::endl;
  if (s.ok()) {
    std::cout << request->a() << " + " << request->b() << " = "
              << TextStyle::Green(base::NumberToString(response->sum()))
              << std::endl;
  } else {
    LOG(ERROR) << s;
  }
}

void GrpcClientNode::RequestAdd() {
  AddRequest* request = new AddRequest();
  AddResponse* response = new AddResponse();
  request->set_a(simple_service_flag_.a_flag()->value());
  request->set_b(simple_service_flag_.b_flag()->value());
  client_->AddAsync(
      request, response,
      base::BindOnce(&GrpcClientNode::OnRequestAdd, base::Unretained(this),
                     base::Owned(request), base::Owned(response)));
}
```

To `Unregister`, it's also very similar.

```c++
void GrpcClientNode::OnRequestUnegister(Status s) {
  std::cout << "GrpcClientNode::OnRequestUnegister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void GrpcClientNode::RequestUnregister() {
  client_.RequestUnregister(node_info_, service_,
                            base::BindOnce(&GrpcClientNode::OnRequestUnegister,
                                           base::Unretained(this)));
}
```

