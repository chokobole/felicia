import { TOPIC_INFO } from '@felicia-viz/proto/messages/master-data';

import Connection from './connection';
import Worker from './subscriber-webworker';

export default class TopicInfoSubscriber {
  constructor(serverAddr, onmessage) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
    this.worker = new Worker();
    this.worker.onmessage = onmessage;
  }

  initialize() {
    this.connection.initialize(
      () => {
        const data = JSON.stringify({
          type: TOPIC_INFO,
        });

        this.connection.ws.send(data);
      },
      event => {
        this.worker.postMessage({
          type: TOPIC_INFO,
          data: event.data,
        });
      }
    );
  }

  close() {
    this.worker.terminate();
    this.connection.markClose();
    this.connection.ws.close();
  }
}
