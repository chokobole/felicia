# ORB SLAM2

## Prerequisite

```bash
git clone https://github.com/chokobole/ORB_SLAM2.git
cd ORB_SLAM2
./build.sh
```

## Build

```bash
bazel build --define has_opencv=true --define has_orb_slam2=true //felicia/examples/slam:slam_node_creator
```

If you want to run with dataset loader,

```bash
bazel build //felicia/examples/slam:dataset_loader_node_creator
```

## Run

## Run Mono

```bash
bazel-bin/felicia/examples/slam/slam_node_creator --slam_kind OrbSlam2 --voc_path /path/to/voc --settings_path /path/to/settings --frame_topic frame --map_topic map --pose_topic pose --left_color_topic left
```

For example, if you want to provide dataset, you can do like below.
```bash
bazel-bin/felicia/examples/slam/dataset_loader_node_creator TUM /path/to/tum --left_color_topic left
```

## Run RGBD

```bash
bazel-bin/felicia/examples/slam/slam_node_creator --slam_kind OrbSlam2 --voc_path /path/to/voc --settings_path /path/to/settings --frame_topic frame --map_topic map --pose_topic pose --left_color_topic left --right_color_topic right
```

For example, if you want to provide dataset, you can do like below.
```bash
bazel-bin/felicia/examples/slam/dataset_loader_node_creator TUM /path/to/tum --left_color_topic left --depth_topic dpeth
```

## Run Stereo

```bash
bazel-bin/felicia/examples/slam/slam_node_creator --slam_kind OrbSlam2 --voc_path /path/to/voc --settings_path /path/to/settings --frame_topic frame --map_topic map --pose_topic pose --left_color_topic left --right_color_topic right
```

For example, if you want to provide dataset, you can do like below.
```bash
bazel-bin/felicia/examples/slam/dataset_loader_node_creator KITTI /path/to/kitti --left_color_topic left --right_color_topic --left_as_gray_scale --right_as_gray_scale
```