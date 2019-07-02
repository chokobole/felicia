# Human Pose Estimation

**Tested on Linux**

## Prerequisite

```bash
git clone https://github.com/CMU-Perceptual-Computing-Lab/openpose.git
```

Follow the [instructions](https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/installation.md#installation).

## Build

```bash
bazel build //felicia/core/master/rpc:grpc_server_main
bazel build //felicia/examples/learn/deep_learning/human_pose_estimation:human_pose_estimation
```

## Run

```bash
export PYTHONPATH=$PYTHONPATH:/usr/local/python
LD_LIBRARY_PATH=/usr/local/lib bazel-bin/felicia/examples/learn/deep_learning/human_pose_estimation/human_pose_estimation --model /path/to/model
```