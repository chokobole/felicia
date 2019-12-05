

![Felicia Logo](docs/resources/felicia-logo.png)

[![Build Status](https://travis-ci.com/chokobole/felicia.svg?token=uWEvhLXsK9nuPxhDRPic&branch=master)](https://travis-ci.com/chokobole/felicia)

## Overview

Felicia is the chromium based robot framework like [ROS](https://www.ros.org/), a collections of tools and libraries to help developers build **fast**, **light** and **secure** robot software. Currently it is under development mostly alone, I think it is still useful for like Robot-vision developer who needs to deliver camera images fast or web developers who wants to subscribe messages by WebSocket without any performance drop. I hope someday it be spread through robot industry and academia so that I want many developers to be engaged with it! If you have any trouble, leave issues or contact to me by email!

**Key** features:

* Cross Platform.
* Support Protobuf / ROS Message.
* Use grpc.
* Provide C++ / Python APIs.
* Provide different communication channels (TCP / UDP / Shared Memory / WebSocket / Unix Domain Socket).
* Provide some of sensor APIs.
  * [Realsense D435i](https://github.com/chokobole/felicia-realsense)
  * [ZED Stereo Camrea](https://github.com/chokobole/felicia-zed)
  * [RPLidar](https://github.com/chokobole/felicia-rplidar)
  * UVC compatible camera
* Provide command-line-interface.
* Provide visualization tool with browser.
* Compatible with ROS1 topic / service protocol.

**TODO** feautures:

* Decentralize nodes.
* Support TLS communicaiton.
* Support OS-layer security.
* Provide more channels such as Bluetooth, QUIC.
* Provide more sensor APIs such as michrophone.
* Provide simulator.
* Compatible with ROS2 topic / service protocol.
* Support Raspberry Pi 4.
* Robot specific features such as SLAM.

## Contents
1. [Installation](docs/installation.md)
2. [Master Server Main](docs/master_server_main.md)
3. [Command Line Interface](docs/command_line_interface.md)
4. [Enviornment Variables](docs/environment_variables.md)
5. [Felicia-Viz](felicia-viz/README.md)
6. [Examples](https://github.com/chokobole/felicia-examples)
7. [Contribution](docs/contribution.md)
