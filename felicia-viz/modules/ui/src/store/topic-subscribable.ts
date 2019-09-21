import Subscriber, { Message } from '@felicia-viz/communication/subscriber';
import { IPEndPointProtobuf } from '@felicia-viz/proto/messages/channel';
import { action, observable } from 'mobx';

export default class TopicSubscribable {
  id: number;

  private subscriber: Subscriber;

  @observable topics: Map<string, string> = new Map();

  constructor(id: number, subscriber: Subscriber) {
    this.id = id;
    this.subscriber = subscriber;
  }

  @action setTopic(typeName: string, topic: string, address: IPEndPointProtobuf): void {
    this.topics.set(typeName, topic);
    this.subscriber.subscribeTopic(this.id, typeName, topic, address);
  }

  @action unsetTopic(typeName: string, topic: string): void {
    this.topics.delete(typeName);
    this.subscriber.unsubscribeTopic(this.id, topic);
  }

  @action clear(): void {
    this.topics.forEach(topic => {
      this.subscriber.unsubscribeTopic(this.id, topic);
    });
    this.topics.clear();
  }

  hasTopic(typeName: string) {
    return this.topics.has(typeName);
  }

  getTopic(typeName: string) {
    return this.topics.get(typeName);
  }

  // eslint-disable-next-line class-method-use-this
  update(_: Message): void {}

  viewType = (): string => {
    return '';
  };
}
