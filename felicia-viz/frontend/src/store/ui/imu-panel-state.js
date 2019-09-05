import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';
import { makeVector3, makeQuarternion } from 'util/babylon-util';

export class ImuFrame {
  constructor(message) {
    const { orientation, angularVelocity, linearAcceleration, timestamp } = message.data;
    this.orientation = makeQuarternion(orientation);
    this.angularVelocity = makeVector3(angularVelocity);
    this.linearAcceleration = makeVector3(linearAcceleration);
    this.timestamp = timestamp;
  }
}

export default class ImuPanelState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new ImuFrame(message);
  }

  type = () => {
    return 'ImuPanel';
  };
}
