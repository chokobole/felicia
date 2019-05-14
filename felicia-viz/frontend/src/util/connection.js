export default class Connection {
  constructor(serverAddr) {
    this.serverAddr = serverAddr;
  }

  initialize(onmessage) {
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

    this.ws.onmessage = onmessage;
    this.ws.onclose = () => {
      if (!this.forceClosed) this.initialize();
    };
  }

  markClose() {
    this.forceClosed = true;
  }
}
