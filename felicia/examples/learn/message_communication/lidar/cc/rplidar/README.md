# Lidar(RPLidar)

**Tested using A1**

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build --define has_rplidar=true //felicia/core/master/rpc:grpc_server_main
bazel build --define has_rplidar=true //felicia/examples/learn/message_communication/lidar/cc:lidar_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

If you want to see the visual result, check out [felicia-viz](/felicia-viz/README.md)!

We don't explain the same we had in [README.md](/felicia/examples/learn/message_communication/protobuf/cc/README.md).

To start you need to input either `ip` and `tcp_port` or `serial_port` and `baudrate`.

Here try with serial communication! Maybe you should change the privilege of `serial_port`.

```bash
sudo chmod 666 /dev/ttyUSB0
```

The default `baudrate` you should check out on the [web](http://www.slamtec.com/en/Support).

```bash
bazel-bin/felicia/examples/learn/message_communication/lidar/cc/lidar_node_creator -t message -p --serial_port /dev/ttyUSB0 --baudrate 115200
```
