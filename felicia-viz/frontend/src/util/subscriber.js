import STORE from 'store';
import Connection from 'util/connection';
import Worker from 'util/subscriber-webworker';

class Subscriber {
  constructor(serverAddr) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
    this.listeners = [];
  }

  initialize(worker, type, topic) {
    this.connection.initialize(this.requestTopic.bind(this, type, topic), event => {
      worker.postMessage({
        type,
        destinations: this.listeners,
        data: event.data,
      });
    });
  }

  requestTopic(type, topic) {
    console.log(`request Topic ${type} ${topic}`);
    const data = JSON.stringify({
      type,
      topic,
    });

    this.connection.ws.send(data);
  }

  close() {
    this.connection.markClose();
    this.connection.ws.close();
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

class SubscriberPool {
  subscribers = new Map();

  constructor() {
    this.worker = new Worker();
    this.worker.onmessage = event => {
      const { data, type, destinations } = event.data;
      destinations.forEach(id => {
        STORE.update({
          type,
          data,
          id,
        });
      });
    };
  }

  subscribeTopic(id, type, topic) {
    this.subscribers.forEach((subscriber, topicKey, map) => {
      if (topicKey !== topic) {
        if (subscriber.removeListener(id)) {
          if (subscriber.listeners.length === 0) {
            map.delete(topicKey);
          }
        }
      }
    });

    let subscriber;
    if (!this.subscribers.has(topic)) {
      subscriber = new Subscriber();
      subscriber.initialize(this.worker, type, topic);
      this.subscribers.set(topic, subscriber);
    } else {
      subscriber = this.subscribers.get(topic);
    }
    subscriber.addListener(id);
  }
}

const SUBSCRIBER = new SubscriberPool();

export default SUBSCRIBER;
