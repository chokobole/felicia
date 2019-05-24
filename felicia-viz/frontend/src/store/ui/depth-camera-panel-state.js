import { observable, action } from 'mobx';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';

import CameraPanelState, { CameraFrame } from './camera-panel-state';

export class DepthCameraFrame extends CameraFrame {
  constructor(message) {
    const { data } = message;
    const { frame, scale } = data;
    super({
      data: frame,
    });
    this.scale = scale;
  }
}

export default class DepthCameraPanelState extends CameraPanelState {
  @observable filter = 'jet';

  @observable pointcloudView = false;

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

  @action setPointcloudView(newPointcloudview) {
    this.pointcloudView = newPointcloudview;
  }

  type = () => {
    return 'DepthCameraPanel';
  };
}
