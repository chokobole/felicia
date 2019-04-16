import STORE from 'store';
import Worker from 'util/webworker.js';

class Subscriber {
  constructor(serverAddr) {
    this.serverAddr = serverAddr;
    this.websocket = null;
    this.worker = new Worker();
  }

  initialize() {
    try {
      this.websocket = new WebSocket(this.serverAddr);
      this.websocket.binaryType = 'arraybuffer';
    } catch (error) {
      console.error(`Failed to establish a connection: ${error}`);
      setTimeout(() => {
        this.initialize();
      }, 1000);
      return;
    }
    this.websocket.onmessage = event => {
      this.worker.postMessage({
        source: 'camera',
        data: event.data,
      });
    };

    this.worker.onmessage = event => {
      STORE.update(event.data);
    };
    this.websocket.onclose = event => {
      console.log(`WebSocket connection closed with code: ${event.code}`);
      this.initialize();
    };
  }
}

export default new Subscriber(`ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`);
