import React from 'react';

import CameraFrameControlPanel from 'components/camera-frame-control-panel';
import CameraFrameView from 'components/camera-frame-view';
import DepthCameraFrameControlPanel from 'components/depth-camera-frame-control-panel';
import DepthCameraFrameView from 'components/depth-camera-frame-view';
import ImageWithBoundingBoxesControlPanel from 'components/image-with-bounding-boxes-control-panel';
import ImageWithBoundingBoxesView from 'components/image-with-bounding-boxes-view';
import ImageWithHumansControlPanel from 'components/image-with-humans-control-panel';
import ImageWithHumansView from 'components/image-with-humans-view';
import ImuFrameControlPanel from 'components/imu-frame-control-panel';
import ImuFrameView from 'components/imu-frame-view';
import LidarFrameControlPanel from 'components/lidar-frame-control-panel';
import LidarFrameView from 'components/lidar-frame-view';
import MainSceneControlPanel from 'components/main-scene-control-panel';
import CameraFrameViewState from 'store/ui/camera-frame-view-state';
import DepthCameraFrameViewState from 'store/ui/depth-camera-frame-view-state';
import ImageWithBoundingBoxesViewState from 'store/ui/image-with-bounding-boxes-view-state';
import ImageWithHumansViewState from 'store/ui/image-with-humans-view-state';
import ImuFrameViewState from 'store/ui/imu-frame-view-state';
import LidarFrameViewState from 'store/ui/lidar-frame-view-state';
import MainSceneState from 'store/ui/main-scene-state';

const UI_TYPES = {};

export const MainSceneType = {
  name: 'MainScene',
  className: 'main-scene',
  state: MainSceneState,
  renderView: () => {
    throw new Error('You called renderView of MainScene.');
  },
  renderControlPanel: () => {
    return <MainSceneControlPanel />;
  },
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

export const ImageWithBoundingBoxesViewType = {
  name: 'ImageWithBoundingBoxesView',
  className: 'image-with-bounding-boxes-view',
  state: ImageWithBoundingBoxesViewState,
  renderView: id => {
    return <ImageWithBoundingBoxesView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <ImageWithBoundingBoxesControlPanel />;
  },
};

UI_TYPES[ImageWithBoundingBoxesViewType.name] = ImageWithBoundingBoxesViewType;

export const ImageWithHumansViewType = {
  name: 'ImageWithHumansView',
  className: 'image-with-humans-view',
  state: ImageWithHumansViewState,
  renderView: id => {
    return <ImageWithHumansView key={id} id={id} />;
  },
  renderControlPanel: () => {
    return <ImageWithHumansControlPanel />;
  },
};

UI_TYPES[ImageWithHumansViewType.name] = ImageWithHumansViewType;

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

export default UI_TYPES;
