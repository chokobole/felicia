import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';
import { toVector3, toQuarternion } from 'util/babylon-util';

export class ImuFrameMessage {
  constructor(message) {
    const { orientation, angularVelocity, linearAcceleration, timestamp } = message.data;
    this.orientation = toQuarternion(orientation);
    this.angularVelocity = toVector3(angularVelocity);
    this.linearAcceleration = toVector3(linearAcceleration);
    this.timestamp = timestamp;
  }
}

export default class ImuPanelState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new ImuFrameMessage(message);
  }

  type = () => {
    return 'ImuPanel';
  };
}
