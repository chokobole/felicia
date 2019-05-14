import MESSAGE_TYPES from 'common/message-type';
import QUERY_TYPES from 'common/query-type';
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
    this.connection.initialize(
      () => {
        const content = JSON.stringify({
          type: MESSAGE_TYPES.MetaInfo.name,
          queryType: QUERY_TYPES.Topics.name,
        });

        this.connection.ws.send(content);
      },
      event => {
        this.worker.postMessage({
          type: MESSAGE_TYPES.MetaInfo.name,
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

const META_INFO_SUBSCRIBER = new MetaInfoSubscriber();

export default META_INFO_SUBSCRIBER;
