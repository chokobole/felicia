import { observable, action } from 'mobx';

import SUBSCRIBER from 'util/subscriber';

export default class PanelState {
  @observable topics = new Map();

  constructor(id) {
    this.id = id;
  }

  @action setTopic(typeName, newTopic) {
    this.topics.set(typeName, newTopic);
    SUBSCRIBER.subscribeTopic(this.id, typeName, newTopic);
  }
}
