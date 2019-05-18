import React from 'react';

import CameraPanel, { CameraControlPanel } from 'components/camera-panel';
import ImuPanel, { ImuControlPanel } from 'components/imu-panel';
import CameraPanelState from 'store/ui/camera-panel-state';
import ImuPanelState from 'store/ui/imu-panel-state';

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

export default UI_TYPES;
