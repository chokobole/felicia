import { observable, action } from 'mobx';

import PanelState from './panel-state';

export class CameraFrame {
  constructor(message) {
    const { data } = message;
    const { converted, cameraFormat, timestamp } = data;
    this.converted = converted;
    this.data = data.data;
    this.timestamp = timestamp;
    const { width, height, pixelFormat, frameRate } = cameraFormat;
    this.width = width;
    this.height = height;
    this.pixelFormat = pixelFormat;
    this.frameRate = frameRate;
  }
}

export default class CameraPanelState extends PanelState {
  @observable frame = null;

  @action update(message) {
    this.frame = new CameraFrame(message);
  }

  type = () => {
    return 'CameraPanel';
  };
}
