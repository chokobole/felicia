import React from 'react';

import CameraFrameControlPanel from '@felicia-viz/ui/components/camera-frame-control-panel';
import CameraFrameView from '@felicia-viz/ui/components/camera-frame-view';
import DepthCameraFrameControlPanel from '@felicia-viz/ui/components/depth-camera-frame-control-panel';
import DepthCameraFrameView from '@felicia-viz/ui/components/depth-camera-frame-view';
import CameraFrameViewState from '@felicia-viz/ui/store/ui/camera-frame-view-state';
import ImuFrameControlPanel from '@felicia-viz/ui/components/imu-frame-control-panel';
import ImuFrameView from '@felicia-viz/ui/components/imu-frame-view';
import LidarFrameControlPanel from '@felicia-viz/ui/components/lidar-frame-control-panel';
import LidarFrameView from '@felicia-viz/ui/components/lidar-frame-view';
import DepthCameraFrameViewState from '@felicia-viz/ui/store/ui/depth-camera-frame-view-state';
import ImuFrameViewState from '@felicia-viz/ui/store/ui/imu-frame-view-state';
import LidarFrameViewState from '@felicia-viz/ui/store/ui/lidar-frame-view-state';
import OccupancyGridMapControlPanel from '@felicia-viz/ui/components/occupancy-grid-map-control-panel';
import PointcloudControlPanel from '@felicia-viz/ui/components/pointcloud-control-panel';
import PoseWithTimestampControlPanel from '@felicia-viz/ui/components/pose-with-timestamp-control-panel';
import Pose3WithTimestampControlPanel from '@felicia-viz/ui/components/pose3-with-timestamp-control-panel';

import CameraControlPanel from 'components/camera-control-panel';
import MainSceneState from 'store/ui/main-scene-state';

const UI_TYPES = {};

export const MainSceneType = {
  name: 'MainScene',
  state: MainSceneState,
};

UI_TYPES[MainSceneType.name] = MainSceneType;

export const CameraFrameViewType = {
  name: 'CameraFrameView',
  className: 'camera-frame-view',
  state: CameraFrameViewState,
  renderView: id => {
    return <CameraFrameView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <CameraFrameControlPanel />;
  },
};

UI_TYPES[CameraFrameViewType.name] = CameraFrameViewType;

export const DepthCameraFrameViewType = {
  name: 'DepthCameraFrameView',
  className: 'depth-camera-frame-view',
  state: DepthCameraFrameViewState,
  renderView: id => {
    return <DepthCameraFrameView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <DepthCameraFrameControlPanel />;
  },
};

UI_TYPES[DepthCameraFrameViewType.name] = DepthCameraFrameViewType;

export const ImuFrameViewType = {
  name: 'ImuFrameView',
  className: 'imu-frame-view',
  state: ImuFrameViewState,
  renderView: id => {
    return <ImuFrameView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <ImuFrameControlPanel />;
  },
};

UI_TYPES[ImuFrameViewType.name] = ImuFrameViewType;

export const LidarFrameViewType = {
  name: 'LidarFrameView',
  className: 'lidar-frame-view',
  state: LidarFrameViewState,
  renderView: id => {
    return <LidarFrameView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <LidarFrameControlPanel />;
  },
};

UI_TYPES[LidarFrameViewType.name] = LidarFrameViewType;

export const CameraControlPanelType = {
  name: 'CameraControlPanel',
  renderControlPanel: () => {
    return <CameraControlPanel />;
  },
};

UI_TYPES[CameraControlPanelType.name] = CameraControlPanelType;

export const OccupancyGridMapControlPanelType = {
  name: 'OccupancyGridMapControlPanel',
  renderControlPanel: () => {
    return <OccupancyGridMapControlPanel />;
  },
};

UI_TYPES[OccupancyGridMapControlPanelType.name] = OccupancyGridMapControlPanelType;

export const PointcloudControlPanelType = {
  name: 'PointcloudControlPanel',
  renderControlPanel: () => {
    return <PointcloudControlPanel />;
  },
};

UI_TYPES[PointcloudControlPanelType.name] = PointcloudControlPanelType;

export const PoseWithTimestampControlPanelType = {
  name: 'PoseWithTimestampControlPanel',
  renderControlPanel: () => {
    return <PoseWithTimestampControlPanel />;
  },
};

UI_TYPES[PoseWithTimestampControlPanelType.name] = PoseWithTimestampControlPanelType;

export const Pose3WithTimestampControlPanelType = {
  name: 'Pose3WithTimestampControlPanel',
  renderControlPanel: () => {
    return <Pose3WithTimestampControlPanel />;
  },
};

UI_TYPES[Pose3WithTimestampControlPanelType.name] = Pose3WithTimestampControlPanelType;

export default UI_TYPES;
