export default class Connection {
  constructor(serverAddr) {
    this.serverAddr = serverAddr;
  }

  initialize(onMessage) {
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

    this.ws.onmessage = onMessage;
    this.ws.onclose = () => {
      if (!this.forceClosed) this.initialize();
    };
  }

  markClose() {
    this.forceClosed = true;
  }
}
