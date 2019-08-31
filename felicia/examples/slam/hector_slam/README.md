# Hector SLAM

This is taken and modified from [Hector SLAM](https://github.com/tu-darmstadt-ros-pkg/hector_slam).

## Build

```bash
bazel build //felicia/examples/slam:slam_node_creator
```

## Run

```bash
bazel-bin/felicia/examples/slam/slam_node_creator --slam_kind HectorSlam --lidar_topic lidar --pose_topic pose --map_topic map
```