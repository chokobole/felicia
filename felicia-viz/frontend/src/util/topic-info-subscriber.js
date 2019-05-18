import { TOPIC_INFO } from '@felicia-viz/communication';

import STORE from 'store';
import Connection from 'util/connection';
import Worker from 'util/subscriber-webworker';

class TopicInfoSubscriber {
  constructor(serverAddr) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
    this.worker = new Worker();
    this.worker.onmessage = event => {
      const { type, data } = event.data;
      STORE.update({
        type,
        data,
      });
    };
  }

  initialize() {
    this.connection.initialize(
      () => {
        const content = JSON.stringify({
          type: TOPIC_INFO,
        });

        this.connection.ws.send(content);
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

const TOPIC_INFO_SUBSCRIBER = new TopicInfoSubscriber();

export default TOPIC_INFO_SUBSCRIBER;
