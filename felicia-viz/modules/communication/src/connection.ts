class MyWebSocket extends WebSocket {
  forceClosed: boolean;

  constructor(serverAddr: string) {
    super(serverAddr);
    this.forceClosed = false;
  }
}

export default class Connection {
  private serverAddr: string;

  ws?: MyWebSocket;

  constructor(serverAddr: string) {
    this.serverAddr = serverAddr;
  }

  initialize(
    onopen: ((this: WebSocket, ev: Event) => void) | null = null,
    onmessage: ((this: WebSocket, ev: MessageEvent) => void) | null = null
  ): void {
    try {
      this.ws = new MyWebSocket(this.serverAddr);
      this.ws.binaryType = 'arraybuffer';
    } catch (error) {
      console.error(`Failed to establish a connection: ${error}`);
      setTimeout(() => {
        this.initialize();
      }, 1000);
      return;
    }

    this.ws.onopen = onopen;
    this.ws.onmessage = onmessage;
    this.ws.onclose = () => {
      if (!this.ws!.forceClosed) this.initialize();
    };
  }

  close(): void {
    this.ws!.forceClosed = true;
    this.ws!.close();
  }
}
