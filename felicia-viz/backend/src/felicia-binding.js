import feliciaJs from 'felicia_js.node';
import TYPES from 'common/connection-type';
import websocket from './websocket';

export default () => {
  const ws = websocket();

  feliciaJs.MasterProxy.setBackground();

  const s = feliciaJs.MasterProxy.start();
  if (!s.ok()) {
    process.exit(1);
  }

  feliciaJs.MasterProxy.requestRegisterDynamicSubscribingNode(
    function(topic, message) {
      console.log(`[TOPIC]: ${topic}`);
      if (message.type === 'felicia.CameraMessage') {
        ws.broadcast(message.message.data, TYPES.Camera);
      } else {
        ws.broadcast(
          JSON.stringify({
            type: message.type,
            data: message.message,
          }),
          TYPES.General
        );
      }
    },
    function(topic, status) {
      console.log(`[TOPIC]: ${topic}`);
      console.error(status.errorMessage());
    },
    {
      period: 100,
      queue_size: 1,
    }
  );

  feliciaJs.MasterProxy.run();
};
