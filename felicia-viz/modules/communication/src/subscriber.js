import Connection from './connection';
import { hasWSChannel } from './proto-types';
import Worker from './subscriber-webworker';

class Subscriber {
  constructor(ipEndpoint) {
    this.updateAddress(ipEndpoint);
    this.listeners = [];
  }

  initialize(worker, type) {
    this.type = type;
    this.connection.initialize(null, event => {
      worker.postMessage({
        type,
        destinations: this.listeners,
        data: event.data,
      });
    });
  }

  updateAddress(ipEndpoint) {
    this.close();
    this.ipEndpoint = ipEndpoint;
    const { ip, port } = ipEndpoint;
    const serverAddr = `ws://${ip}:${port}`;
    this.connection = new Connection(serverAddr);
  }

  close() {
    if (this.connection) this.connection.close();
  }

  addListener(id) {
    this.listeners.push(id);
  }

  removeListener(id) {
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
  constructor(onmessage) {
    this.worker = new Worker();
    this.worker.onmessage = onmessage;
    this.subscribers = new Map();
  }

  subscribeTopic(id, type, topic, address) {
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
    subscriber.addListener(id);
  }

  unsubscribeTopic(id, topic) {
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

  updateTopics(newTopics) {
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
