import { observable, action } from 'mobx';

import MESSAGE_TYPES from 'common/message-type';
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
    SUBSCRIBER.subscribeTopic(this.id, MESSAGE_TYPES.Imu.name, newTopic);
  }

  type = () => {
    return 'ImuPanel';
  };
}
