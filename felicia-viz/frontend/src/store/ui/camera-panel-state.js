import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';

export class CameraFrameMessage {
  constructor(message) {
    const { converted, data, cameraFormat, timestamp } = message.data;
    this.converted = converted;
    this.data = data;
    this.timestamp = timestamp;
    const { size, pixelFormat, frameRate } = cameraFormat;
    const { width, height } = size;
    this.width = width;
    this.height = height;
    this.pixelFormat = pixelFormat;
    this.frameRate = frameRate;
  }
}

export default class CameraPanelState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new CameraFrameMessage(message);
  }

  type = () => {
    return 'CameraPanel';
  };
}
