import { observable, action } from 'mobx';

import MESSAGE_TYPES from 'common/message-type';
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
    SUBSCRIBER.subscribeTopic(this.id, MESSAGE_TYPES.Camera.name, newTopic);
  }

  @action selectFilter(newFilter) {
    this.filter = newFilter;
  }

  type = () => {
    return 'CameraPanel';
  };
}
