import { observable, action } from 'mobx';

import ImuFrameMessage from '@felicia-viz/proto/messages/imu-frame-message';

import TopicSubscribable from '../topic-subscribable';

class History {
  constructor(size) {
    this.size = size;
    this.data = {
      x: [{ x: 0, y: 0 }],
      y: [{ x: 0, y: 0 }],
      z: [{ x: 0, y: 0 }],
    };
  }

  push(vec, timestamp) {
    const { x, y, z } = vec;

    if (x === 0 && y === 0 && z === 0) return;

    if (this.data.x.length === this.size) {
      this.data.x.shift();
      this.data.y.shift();
      this.data.z.shift();
    }

    this.data.x.push({ x: timestamp, y: x });
    this.data.y.push({ x: timestamp, y });
    this.data.z.push({ x: timestamp, y: z });
  }

  history() {
    const { x, y, z } = this.data;
    return {
      x,
      y,
      z,
    };
  }
}

export default class ImuFrameViewState extends TopicSubscribable {
  @observable frame = null;

  @observable angularVelocities = new History(100);

  @observable linearAccelerations = new History(100);

  @action update(message) {
    this.frame = new ImuFrameMessage(message.data);
    const { angularVelocity, linearAcceleration, timestamp } = this.frame;
    this.angularVelocities.push(angularVelocity, timestamp);
    this.linearAccelerations.push(linearAcceleration, timestamp);
  }

  viewType = () => {
    return 'ImuFrameView';
  };
}
