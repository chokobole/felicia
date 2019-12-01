# Master Server Main

```bash
bazel run //felicia/core/master/rpc/master_server_main
```

## To communication ROS1 Topic / Service

```bash
bazel run --define has_ros=true //felicia/core/master/rpc/master_server_main
```