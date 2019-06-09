import { observable, action } from 'mobx';

import { makeVector3, makeQuarternion } from 'util/babylon-util';
import PanelState from './panel-state';

export class ImuFrame {
  constructor(message) {
    const { data } = message;
    const { orientation, angularVelocity, linearAcceleration, timestamp } = data;
    this.orientation = makeQuarternion(orientation);
    this.angularVelocity = makeVector3(angularVelocity);
    this.linearAcceleration = makeVector3(linearAcceleration);
    this.timestamp = timestamp;
  }
}

export default class ImuPanelState extends PanelState {
  @observable frame = null;

  @action update(message) {
    this.frame = new ImuFrame(message);
  }

  type = () => {
    return 'ImuPanel';
  };
}
