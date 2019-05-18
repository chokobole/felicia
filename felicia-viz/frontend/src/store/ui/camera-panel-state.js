import { observable, action } from 'mobx';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';

import SUBSCRIBER from 'util/subscriber';

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

export default class CameraPanelState {
  @observable topic = '';

  @observable filter = 'jet';

  @observable frame = null;

  constructor(id) {
    this.id = id;
  }

  @action update(message) {
    this.frame = new CameraFrame(message);
  }

  @action selectTopic(newTopic) {
    this.topic = newTopic;
    SUBSCRIBER.subscribeTopic(this.id, CAMERA_FRAME_MESSAGE, newTopic);
  }

  @action selectFilter(newFilter) {
    this.filter = newFilter;
  }

  type = () => {
    return 'CameraPanel';
  };
}
