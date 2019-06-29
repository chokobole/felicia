import React from 'react';

import CameraPanel, { CameraControlPanel } from 'components/camera-panel';
import DepthCameraPanel, { DepthCameraControlPanel } from 'components/depth-camera-panel';
import ImageWithBoundingBoxesPanel, {
  ImageWithBoundingBoxesControlPanel,
} from 'components/image-with-bounding-boxes-panel';
import ImuPanel, { ImuControlPanel } from 'components/imu-panel';
import LidarPanel, { LidarControlPanel } from 'components/lidar-panel';
import PointcloudPanel, { PointcloudControlPanel } from 'components/pointcloud-panel';
import CameraPanelState from 'store/ui/camera-panel-state';
import DepthCameraPanelState from 'store/ui/depth-camera-panel-state';
import ImageWithBoundingBoxesPanelState from 'store/ui/image-with-bounding-boxes-panel-state';
import ImuPanelState from 'store/ui/imu-panel-state';
import LidarPanelState from 'store/ui/lidar-panel-state';
import PointcloudPanelState from 'store/ui/pointcloud-panel-state';

const UI_TYPES = {};

export const CameraPanelType = {
  name: 'CameraPanel',
  className: 'camera-panel',
  state: CameraPanelState,
  renderView: id => {
    return <CameraPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <CameraControlPanel />;
  },
};

UI_TYPES[CameraPanelType.name] = CameraPanelType;

export const DepthCameraPanelType = {
  name: 'DepthCameraPanel',
  className: 'depth-camera-panel',
  state: DepthCameraPanelState,
  renderView: id => {
    return <DepthCameraPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <DepthCameraControlPanel />;
  },
};

UI_TYPES[DepthCameraPanelType.name] = DepthCameraPanelType;

export const ImageWithBoundingBoxesPanelType = {
  name: 'ImageWithBoundingBoxesPanel',
  className: 'image-with-bounding-boxes-panel',
  state: ImageWithBoundingBoxesPanelState,
  renderView: id => {
    return <ImageWithBoundingBoxesPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <ImageWithBoundingBoxesControlPanel />;
  },
};

UI_TYPES[ImageWithBoundingBoxesPanelType.name] = ImageWithBoundingBoxesPanelType;

export const ImuPanelType = {
  name: 'ImuPanel',
  className: 'imu-panel',
  state: ImuPanelState,
  renderView: id => {
    return <ImuPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <ImuControlPanel />;
  },
};

UI_TYPES[ImuPanelType.name] = ImuPanelType;

export const LidarPanelType = {
  name: 'LidarPanel',
  className: 'lidar-panel',
  state: LidarPanelState,
  renderView: id => {
    return <LidarPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <LidarControlPanel />;
  },
};

UI_TYPES[LidarPanelType.name] = LidarPanelType;

export const PointcloudPanelType = {
  name: 'PointcloudPanel',
  className: 'pointcloud-panel',
  state: PointcloudPanelState,
  renderView: id => {
    return <PointcloudPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <PointcloudControlPanel />;
  },
};

UI_TYPES[PointcloudPanelType.name] = PointcloudPanelType;

export default UI_TYPES;
