import React from 'react';

import CameraPanel, { CameraControlPanel } from 'components/camera-panel';
import DepthCameraPanel, { DepthCameraControlPanel } from 'components/depth-camera-panel';
import ImageWithBoundingBoxesPanel, {
  ImageWithBoundingBoxesControlPanel,
} from 'components/image-with-bounding-boxes-panel';
import ImageWithHumansPanel, {
  ImageWithHumansControlPanel,
} from 'components/image-with-humans-panel';
import ImuPanel, { ImuControlPanel } from 'components/imu-panel';
import LidarPanel, { LidarControlPanel } from 'components/lidar-panel';
import MainSceneControlPanel from 'components/main-scene-control-panel';
import CameraViewState from 'store/ui/camera-view-state';
import DepthCameraViewState from 'store/ui/depth-camera-view-state';
import ImageWithBoundingBoxesViewState from 'store/ui/image-with-bounding-boxes-view-state';
import ImageWithHumansViewState from 'store/ui/image-with-humans-view-state';
import ImuViewState from 'store/ui/imu-view-state';
import LidarViewState from 'store/ui/lidar-view-state';
import MainSceneState from 'store/ui/main-scene-state';

const UI_TYPES = {};

export const MainSceneType = {
  name: 'MainScene',
  className: 'main-scene',
  state: MainSceneState,
  renderView: () => {
    throw new Error('You called renderView of MainScene.');
  },
  renderControlView: () => {
    return <MainSceneControlPanel />;
  },
};

UI_TYPES[MainSceneType.name] = MainSceneType;

export const CameraViewType = {
  name: 'CameraView',
  className: 'camera-panel',
  state: CameraViewState,
  renderView: id => {
    return <CameraPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <CameraControlPanel />;
  },
};

UI_TYPES[CameraViewType.name] = CameraViewType;

export const DepthCameraViewType = {
  name: 'DepthCameraView',
  className: 'depth-camera-panel',
  state: DepthCameraViewState,
  renderView: id => {
    return <DepthCameraPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <DepthCameraControlPanel />;
  },
};

UI_TYPES[DepthCameraViewType.name] = DepthCameraViewType;

export const ImageWithBoundingBoxesViewType = {
  name: 'ImageWithBoundingBoxesView',
  className: 'image-with-bounding-boxes-panel',
  state: ImageWithBoundingBoxesViewState,
  renderView: id => {
    return <ImageWithBoundingBoxesPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <ImageWithBoundingBoxesControlPanel />;
  },
};

UI_TYPES[ImageWithBoundingBoxesViewType.name] = ImageWithBoundingBoxesViewType;

export const ImageWithHumansViewType = {
  name: 'ImageWithHumansView',
  className: 'image-with-humans-panel',
  state: ImageWithHumansViewState,
  renderView: id => {
    return <ImageWithHumansPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <ImageWithHumansControlPanel />;
  },
};

UI_TYPES[ImageWithHumansViewType.name] = ImageWithHumansViewType;

export const ImuViewType = {
  name: 'ImuView',
  className: 'imu-panel',
  state: ImuViewState,
  renderView: id => {
    return <ImuPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <ImuControlPanel />;
  },
};

UI_TYPES[ImuViewType.name] = ImuViewType;

export const LidarViewType = {
  name: 'LidarView',
  className: 'lidar-panel',
  state: LidarViewState,
  renderView: id => {
    return <LidarPanel key={id} id={id} />;
  },
  renderControlView: () => {
    return <LidarControlPanel />;
  },
};

UI_TYPES[LidarViewType.name] = LidarViewType;

export default UI_TYPES;
