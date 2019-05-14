import feliciaJs from 'felicia_js.node';
import { isWin } from 'lib/environment';
import MasterProxyClient from 'master-proxy-client';
import handleMessage, { handleClose } from 'message';
import TOPIC_MAP from 'topic-map';
import websocket from 'websocket';

export default () => {
  const ws = websocket(
    (connection, message) => {
      handleMessage(connection, ws, message);
    },
    connection => {
      handleClose(connection);
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
        TOPIC_MAP.set(topicInfo.topic, topicInfo);
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
