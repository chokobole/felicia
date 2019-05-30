import WebSocket from 'ws';

function noop() {}

export default class Connection {
  constructor(ws, onmessage, onclose) {
    this.ws = ws;
    this.type = null;

    this.ws.on('message', message => {
      onmessage(this, message);
    });

    this.ws.on('close', () => {
      onclose();
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
