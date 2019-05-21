import { observable, action } from 'mobx';
import { Quaternion, Vector3 } from '@babylonjs/core/Maths/math';

import { IMU_MESSAGE } from '@felicia-viz/communication';

import SUBSCRIBER from 'util/subscriber';

function makeVector3(v) {
  const { x, y, z } = v;
  return new Vector3(x, y, z);
}

function makeQuarternion(q) {
  const { x, y, z, w } = q;
  return new Quaternion(x, y, z, w);
}

export class Imu {
  constructor(message) {
    const { data } = message;
    const { orientation, angularVelocity, linearAcceleration, timestamp } = data;
    this.orientation = makeQuarternion(orientation);
    this.angularVelocity = makeVector3(angularVelocity);
    this.linearAcceleration = makeVector3(linearAcceleration);
    this.timestamp = timestamp;
  }
}

export default class ImuPanelState {
  @observable topic = '';

  @observable imu = null;

  constructor(id) {
    this.id = id;
  }

  @action update(message) {
    this.imu = new Imu(message);
  }

  @action selectTopic(newTopic) {
    this.topic = newTopic;
    SUBSCRIBER.subscribeTopic(this.id, IMU_MESSAGE, newTopic);
  }

  type = () => {
    return 'ImuPanel';
  };
}
