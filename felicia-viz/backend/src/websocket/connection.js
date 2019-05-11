import WebSocket from 'ws';

import CONNECTION_TYPES from 'common/connection-type';

function noop() {}

export default class Connection {
  constructor(ws) {
    this.ws = ws;
    this.type = CONNECTION_TYPES.General;

    this.ws.on('message', message => {
      let data;
      try {
        data = JSON.parse(message);
      } catch (e) {
        console.error(e);
        return;
      }
      const { type, topic } = data;
      if (type === CONNECTION_TYPES.General.name) {
        this.type = CONNECTION_TYPES.General;
      } else if (type === CONNECTION_TYPES.Camera.name) {
        this.type = CONNECTION_TYPES.Camera;
        this.topic = topic;
      }
    });

    this.ws.on('pong', () => {
      this.heartbeat();
    });
    this.isTerminated = false;
    this.heartbeat();
    this.checkHeartbeat();
  }

  send(topic, data, type) {
    if (this.ws.readyState === WebSocket.OPEN && type === this.type) {
      if (this.type === CONNECTION_TYPES.Camera) {
        if (!(data instanceof Buffer)) {
          console.error(`Data should be Buffer but got ${typeof data}`);
          return;
        }
        if (this.topic !== topic) return;
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
