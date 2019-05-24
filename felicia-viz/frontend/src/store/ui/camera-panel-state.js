import { observable, action } from 'mobx';

import PanelState from './panel-state';

export class CameraFrame {
  constructor(message) {
    const { data } = message;
    this.frame = data;
  }

  get data() {
    return this.frame.data;
  }

  get width() {
    return this.frame.cameraFormat.width;
  }

  get height() {
    return this.frame.cameraFormat.height;
  }

  get pixelFormat() {
    return this.frame.cameraFormat.pixelFormat;
  }

  get frameRate() {
    return this.frame.cameraFormat.frameRate;
  }

  get timestamp() {
    return this.frame.timestamp;
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
