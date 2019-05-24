import { observable, action } from 'mobx';

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

  @action update(message) {
    this.frame = new DepthCameraFrame(message);
  }

  @action selectFilter(newFilter) {
    this.filter = newFilter;
  }

  @action switchPointcloudView(newPointcloudview) {
    this.pointcloudView = newPointcloudview;
  }

  type = () => {
    return 'DepthCameraPanel';
  };
}
