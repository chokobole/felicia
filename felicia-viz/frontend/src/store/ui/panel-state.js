import { observable, action } from 'mobx';

import SUBSCRIBER from 'util/subscriber';

export default class PanelState {
  @observable topics = new Map();

  constructor(id) {
    this.id = id;
  }

  @action setTopic(typeName, topic, address) {
    this.topics.set(typeName, topic);
    SUBSCRIBER.subscribeTopic(this.id, typeName, topic, address);
  }
}
