import { observable, action } from 'mobx';

import SUBSCRIBER from 'util/subscriber';

export default class TopicSubscribable {
  @observable topics = new Map();

  constructor(id) {
    this.id = id;
  }

  @action setTopic(typeName, topic, address) {
    this.topics.set(typeName, topic);
    SUBSCRIBER.subscribeTopic(this.id, typeName, topic, address);
  }

  @action unsetTopic(typeName, topic) {
    this.topics.delete(typeName);
    SUBSCRIBER.unsubscribeTopic(this.id, topic);
  }
}
