export default class Connection {
  constructor(serverAddr) {
    this.serverAddr = serverAddr;
  }

  initialize(type, onMessage) {
    try {
      this.ws = new WebSocket(this.serverAddr);
      this.ws.binaryType = 'arraybuffer';
    } catch (error) {
      console.error(`Failed to establish a connection: ${error}`);
      setTimeout(() => {
        this.initialize();
      }, 1000);
      return;
    }

    this.ws.onopen = () => {
      this.ws.send(type);
    };
    this.ws.onmessage = onMessage;
    this.ws.onclose = () => {
      this.initialize();
    };
  }

  close() {
    this.ws.close();
  }
}
