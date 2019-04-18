import Connection from 'util/connection';

export default class Subscriber {
  constructor(serverAddr) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
  }

  initialize(type, onMessage) {
    this.connection.initialize(type, onMessage);
  }

  close() {
    this.connection.close();
  }
}
