import Connection from 'util/connection';

export default class Subscriber {
  constructor(serverAddr) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
  }

  initialize(onMessage) {
    this.connection.initialize(onMessage);
  }

  requestTopic(type, topic) {
    console.log(`request Topic ${type} ${topic}`);
    const data = JSON.stringify({
      type,
      topic,
    });
    this.connection.ws.send(data);
  }

  close() {
    this.connection.close();
  }
}
