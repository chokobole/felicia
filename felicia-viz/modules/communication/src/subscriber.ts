import { IPEndPointProtobuf } from '@felicia-viz/proto/messages/channel';
import { hasWSChannel, TopicInfoProtobuf } from '@felicia-viz/proto/messages/master-data';
import Connection from './connection';
import Worker from './subscriber-webworker';

export interface Message {
  type: string;
  data: any;
  id: number;
}

export class Subscriber {
  listeners: number[];

  type?: string;

  connection?: Connection;

  constructor(ipEndpoint: IPEndPointProtobuf) {
    this.updateAddress(ipEndpoint);
    this.listeners = [];
  }

  initialize(worker: Worker, type: string): void {
    this.type = type;
    this.connection!.initialize(null, event => {
      worker.postMessage({
        type,
        destinations: this.listeners,
        data: event.data,
      });
    });
  }

  updateAddress(ipEndpoint: IPEndPointProtobuf): void {
    this.close();
    const { ip, port } = ipEndpoint;
    const serverAddr = `ws://${ip}:${port}`;
    this.connection = new Connection(serverAddr);
  }

  close(): void {
    if (this.connection) this.connection.close();
  }

  addListener(id: number): void {
    this.listeners.push(id);
  }

  removeListener(id: number): boolean {
    const idx = this.listeners.findIndex(listener => {
      return listener === id;
    });
    if (idx > -1) {
      this.listeners.splice(idx, 1);
      if (this.listeners.length === 0) {
        this.close();
      }
      return true;
    }

    return false;
  }
}

export default class SubscriberPool {
  private worker: Worker;

  private subscribers: Map<string, Subscriber>;

  constructor(onmessage: (this: Worker, ev: MessageEvent) => any) {
    this.worker = new Worker();
    this.worker.onmessage = onmessage;
    this.subscribers = new Map<string, Subscriber>();
  }

  subscribeTopic(id: number, type: string, topic: string, address: IPEndPointProtobuf): void {
    this.subscribers.forEach((subscriber, topicKey, map) => {
      if (subscriber.type === type) {
        if (subscriber.removeListener(id)) {
          console.log(`${id} stop listening ${topic}`);
          if (subscriber.listeners.length === 0) {
            subscriber.close();
            map.delete(topicKey);
            console.log(`unsubscribe ${topicKey}`);
          }
        }
      }
    });

    let subscriber;
    if (!this.subscribers.has(topic)) {
      subscriber = new Subscriber(address);
      subscriber.initialize(this.worker, type);
      this.subscribers.set(topic, subscriber);
    } else {
      subscriber = this.subscribers.get(topic);
    }
    subscriber!.addListener(id);
  }

  unsubscribeTopic(id: number, topic: string): void {
    this.subscribers.forEach((subscriber, topicKey, map) => {
      if (topicKey === topic) {
        if (subscriber.removeListener(id)) {
          console.log(`${id} stop listening ${topic}`);
          if (subscriber.listeners.length === 0) {
            subscriber.close();
            map.delete(topicKey);
            console.log(`unsubscribe ${topicKey}`);
          }
        }
      }
    });
  }

  updateTopics(newTopics: Array<TopicInfoProtobuf>): void {
    this.subscribers.forEach((subscriber, topicKey, map) => {
      let found = false;
      newTopics.forEach(value => {
        found = hasWSChannel(value);
      });

      if (!found) {
        subscriber.close();
        map.delete(topicKey);
      }
    });
  }
}
