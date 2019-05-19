import Connection from './connection';
import Worker from './subscriber-webworker';

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

export default class SubscriberPool {
  constructor(serverAddr, onmessage) {
    this.serverAddr = serverAddr;
    this.worker = new Worker();
    this.worker.onmessage = onmessage;
    this.subscribers = new Map();
  }

  subscribeTopic(id, type, topic) {
    this.subscribers.forEach((subscriber, topicKey, map) => {
      if (topicKey !== topic) {
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
      subscriber = new Subscriber(this.serverAddr);
      subscriber.initialize(this.worker, type, topic);
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
}
