import MESSAGE_TYPES from 'common/message-type';
import STORE from 'store';
import Connection from 'util/connection';
import Worker from 'util/subscriber-webworker';

class MetaInfoSubscriber {
  constructor(serverAddr) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
    this.worker = new Worker();
    this.worker.onmessage = event => {
      const { type, queryType, data } = event.data;
      STORE.update({
        type,
        queryType,
        data,
      });
    };
  }

  initialize() {
    this.connection.initialize(null, event => {
      this.worker.postMessage({
        type: MESSAGE_TYPES.MetaInfo.name,
        data: event.data,
      });
    });
  }

  request(queryType) {
    if (this.connection.ws.readyState !== WebSocket.OPEN) {
      this.timeout = setTimeout(this.request.bind(this), 100, queryType);
      return;
    }
    const content = JSON.stringify({
      type: MESSAGE_TYPES.MetaInfo.name,
      queryType,
    });

    this.connection.ws.send(content);
  }

  close() {
    this.worker.terminate();
    this.connection.markClose();
    this.connection.ws.close();
    if (this.timeout) {
      clearTimeout(this.timeout);
    }
  }
}

const META_INFO_SUBSCRIBER = new MetaInfoSubscriber();
META_INFO_SUBSCRIBER.initialize();

export default META_INFO_SUBSCRIBER;
