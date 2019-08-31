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

## Run

```bash
bazel-bin/felicia/examples/slam/slam_node_creator --slam_kind OrbSlam2 --voc_path /path/to/voc --settings_path /path/to/settings --frame_topic frame --map_topic map --pose_topic pose --left_color_topic left --right_color_topic right
```