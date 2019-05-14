import WebSocket from 'ws';

import MESSAGE_TYPES from 'common/message-type';

function noop() {}

export default class Connection {
  constructor(ws, onmessage) {
    this.ws = ws;
    this.type = null;

    this.ws.on('message', message => {
      onmessage(this, message);
    });

    this.ws.on('pong', () => {
      this.heartbeat();
    });
    this.isTerminated = false;
    this.heartbeat();
    this.checkHeartbeat();
  }

  send(topic, data, type) {
    if (this.ws.readyState === WebSocket.OPEN && type === this.type && topic === this.topic) {
      if (this.type === MESSAGE_TYPES.Camera) {
        if (!(data instanceof Buffer)) {
          console.error(`Data should be Buffer but got ${typeof data}`);
          return;
        }
      }
      this.ws.send(data);
    }
  }

  heartbeat() {
    this.isAlive = true;
  }

  checkHeartbeat() {
    if (!this.isAlive) {
      this.ws.terminate();
      console.log('Failed to heartbeat, closing socket...');
      return;
    }

    this.isAlive = false;
    this.ws.ping(noop);
    setTimeout(this.checkHeartbeat.bind(this), HEARTBEAT_INTERVAL);
  }

  closed() {
    return this.ws.readyState === WebSocket.CLOSED;
  }
}
