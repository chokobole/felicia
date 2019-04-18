import WebSocket from 'ws';

import TYPES from 'common/connection-type';

function noop() {}

export default class Connection {
  constructor(ws) {
    this.ws = ws;
    this.type = TYPES.General;

    this.ws.on('message', message => {
      if (message === TYPES.General.name) {
        this.type = TYPES.General;
      } else if (message === TYPES.Camera.name) {
        this.type = TYPES.Camera;
      }
    });

    this.ws.on('pong', () => {
      this.heartbeat();
    });
    this.isTerminated = false;
    this.heartbeat();
    this.checkHeartbeat();
  }

  send(data, type) {
    if (this.ws.readyState === WebSocket.OPEN && type === this.type) {
      if (this.type === TYPES.Camera) {
        if (!(data instanceof ArrayBuffer)) {
          console.error(`Data should be ArrayBuffer but got ${typeof data}`);
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
