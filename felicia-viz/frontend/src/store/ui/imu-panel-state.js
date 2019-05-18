import { observable, action } from 'mobx';

import { IMU_MESSAGE } from '@felicia-viz/communication';

import SUBSCRIBER from 'util/subscriber';

export class Imu {}

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
