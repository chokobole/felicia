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

  initialize(onMessage) {
    this.connection.initialize(onMessage);
  }

  requestTopic(type, topic) {
    if (this.connection.ws.readyState !== WebSocket.OPEN) {
      this.timeout = setTimeout(this.requestTopic.bind(this), 100, type, topic);
      return;
    }
    console.log(`request Topic ${type} ${topic}`);
    const data = JSON.stringify({
      type,
      topic,
    });

    this.timeout = null;
    this.connection.ws.send(data);
  }

  close() {
    this.connection.markClose();
    this.connection.ws.close();
    if (this.timeout) {
      clearTimeout(this.timeout);
    }
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
  subscribers = {};

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
    Object.entries(this.subscribers).forEach(([topicKey, subscriber]) => {
      if (topicKey !== topic) {
        if (subscriber.removeListener(id)) {
          if (subscriber.listeners.length === 0) {
            this.subscribers[topicKey] = undefined;
          }
        }
      }
    });

    let subscriber = this.subscribers[topic];
    if (!subscriber) {
      subscriber = new Subscriber();
      this.subscribers[topic] = subscriber;
    }
    subscriber.addListener(id);
    subscriber.initialize(event => {
      this.worker.postMessage({
        type,
        destinations: subscriber.listeners,
        data: event.data,
      });
    });
    this.subscribers[topic].requestTopic(type, topic);
  }
}

const SUBSCRIBER = new SubscriberPool();

export default SUBSCRIBER;
