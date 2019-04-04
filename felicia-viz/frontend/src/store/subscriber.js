import STORE from 'store';

class Subscriber {
  constructor(serverAddr) {
    this.serverAddr = serverAddr;
    this.websocket = null;
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
      STORE.update({
        currentTime: parseInt(event.data, 10),
      });
    };
    this.websocket.onclose = event => {
      console.log(`WebSocket connection closed with code: ${event.code}`);
      this.initialize();
    };
  }
}

export default new Subscriber('ws://localhost:3001');
