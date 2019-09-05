import { observable, action } from 'mobx';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';

import CameraPanelState, { CameraFrame } from './camera-panel-state';

export class DepthCameraFrame extends CameraFrame {
  constructor(message) {
    const { data, cameraFormat, timestamp, min, max } = message.data;
    super({
      data: {
        data,
        cameraFormat,
        timestamp,
      },
    });
    this.min = min;
    this.max = max;
  }
}

export default class DepthCameraPanelState extends CameraPanelState {
  @observable filter = 'jet';

  @observable topicToAlign = '';

  @observable frameToAlign = null;

  @action update(message) {
    if (message.type === CAMERA_FRAME_MESSAGE) {
      this.frameToAlign = new CameraFrame(message);
    } else {
      this.frame = new DepthCameraFrame(message);
    }
  }

  @action setFilter(newFilter) {
    this.filter = newFilter;
  }

  type = () => {
    return 'DepthCameraPanel';
  };
}
