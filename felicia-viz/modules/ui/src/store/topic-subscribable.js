import { observable, action } from 'mobx';

export default class TopicSubscribable {
  @observable topics = new Map();

  constructor(id, subscriber) {
    this.id = id;
    this.subscriber = subscriber;
  }

  @action setTopic(typeName, topic, address) {
    this.topics.set(typeName, topic);
    this.subscriber.subscribeTopic(this.id, typeName, topic, address);
  }

  @action unsetTopic(typeName, topic) {
    this.topics.delete(typeName);
    this.subscriber.unsubscribeTopic(this.id, topic);
  }

  @action clear() {
    this.topics.forEach(topic => {
      this.subscriber.unsubscribeTopic(this.id, topic);
    });
    this.topics.clear();
  }
}
