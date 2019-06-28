# Object Detection

## Prerequisite

```bash
git clone https://github.com/tensorflow/models.git
```

Follow the [instructions](https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/installation.md).

## Build

```bash
bazel build //felicia/core/master/rpc:grpc_server_main
bazel build //felicia/examples/learn/deep_learning/object_detection:object_detection_from_camera
```

## Run

```bash
bazel-bin/felicia/examples/learn/deep_learning/object_detection/object_detection_from_camera
```