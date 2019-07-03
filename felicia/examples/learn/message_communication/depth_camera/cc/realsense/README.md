# Depth Camera(Realsense)

**Tested using D435i**

**Supported on Windows and Linux**

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build --define has_realsense=true //felicia/core/master/rpc:grpc_server_main
bazel build --define has_realsense=true //felicia/examples/learn/message_communication/depth_camera/cc:depth_camera_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

If you want to see the visual result, check out [felicia-viz](/felicia-viz/README.md)!

We don't explain the same we had in [README.md](/felicia/examples/learn/message_communication/camera/cc/README.md).

```bash
bazel-bin/felicia/examples/learn/message_communication/depth_camera/cc/depth_camera_node_creator -l
# For example on linux
device_list is on, it just shows a list of camera devices. If you pass -i(--device_index) with the -l then you can iterate the camera formats the device supports.
[0] display_name: Intel RealSense D435I device_id: /sys/devices/pci0000:00/0000:00:14.0/usb2/2-7/2-7.2/2-7.2.1/2-7.2.1:1.0/video4linux/video0 model_id: 0B3A
```

If you run with the program by passing `-l -i 0`.

```bash
Depth(0)
[0] 1280x720(PIXEL_FORMAT_Z16) 30.00Hz
[1] 1280x720(PIXEL_FORMAT_Z16) 15.00Hz
[2] 1280x720(PIXEL_FORMAT_Z16) 6.00Hz
[3] 848x480(PIXEL_FORMAT_Z16) 90.00Hz
...
[24] 424x240(PIXEL_FORMAT_Z16) 60.00Hz
[25] 424x240(PIXEL_FORMAT_Z16) 30.00Hz
[26] 424x240(PIXEL_FORMAT_Z16) 15.00Hz
[27] 424x240(PIXEL_FORMAT_Z16) 6.00Hz
Color(0)
[0] 1920x1080(PIXEL_FORMAT_RGB) 30.00Hz
[1] 1920x1080(PIXEL_FORMAT_UNKNOWN) 30.00Hz
[2] 1920x1080(PIXEL_FORMAT_Y16) 30.00Hz
[3] 1920x1080(PIXEL_FORMAT_BGRA) 30.00Hz
...
[189] 320x180(PIXEL_FORMAT_BGRA) 6.00Hz
[190] 320x180(PIXEL_FORMAT_ARGB) 6.00Hz
[191] 320x180(PIXEL_FORMAT_BGR) 6.00Hz
[192] 320x180(PIXEL_FORMAT_YUY2) 6.00Hz
Gyro(0)
[0] 200.00Hz
[1] 400.00Hz
Accel(0)
[0] 63.00Hz
[1] 250.00Hz
```

Unlike camera example, there's a lot to control with. If you want to turn on `imu` sensor, you have to pass `--imu_topic`. But watch out that if you publish imu topic as it emits, the other streams can be starved. Also `realsense2` library provide us with synchronously accepting color and depth stream. You can employ this feature by passing `--synched`.