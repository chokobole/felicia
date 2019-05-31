import { PROTO_TYPES, TOPIC_INFO } from '@felicia-viz/communication';

import feliciaJs from 'felicia_js.node';
import { isWin } from 'lib/environment';
import MasterProxyClient from 'master-proxy-client';
import handleMessage, { handleClose } from 'message';
import TOPIC_MAP from 'topic-map';
import websocket from 'websocket';

export default () => {
  const ws = websocket(
    (connection, message) => {
      handleMessage(connection, message);
    },
    () => {
      handleClose();
    }
  );
  feliciaJs.MasterProxy.setBackground();

  if (isWin) {
    global.MasterProxyClient = MasterProxyClient;
    feliciaJs.MasterProxy.startGrpcMasterClient();
  }

  const s = feliciaJs.MasterProxy.start();
  if (!s.ok()) {
    console.error(s.errorMessage());
    process.exit(1);
  }

  function requestRegisterTopicInfoWatcherNode() {
    if (isWin) {
      if (!feliciaJs.MasterProxy.isClientInfoSet()) {
        setTimeout(requestRegisterTopicInfoWatcherNode, 1000);
        return;
      }
    }

    feliciaJs.MasterProxy.requestRegisterTopicInfoWatcherNode(
      message => {
        console.log(`[TOPIC] : ${JSON.stringify(message.message)}`);
        const topicInfo = message.message;
        if (topicInfo.status === PROTO_TYPES[TOPIC_INFO].Status.REGISTERED) {
          TOPIC_MAP.set(topicInfo.topic, topicInfo);
        } else {
          TOPIC_MAP.delete(topicInfo.topic);
        }
        const topics = [];
        TOPIC_MAP.forEach(value => {
          topics.push(value);
        });

        ws.broadcast(
          JSON.stringify({
            type: TOPIC_INFO,
            data: topics,
          }),
          TOPIC_INFO
        );
      },
      // eslint-disable-next-line no-shadow
      s => {
        console.error(s.errorMessage());
        process.exit(1);
      }
    );
  }

  requestRegisterTopicInfoWatcherNode();

  feliciaJs.MasterProxy.run();
};
