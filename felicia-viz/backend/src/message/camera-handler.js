import { CameraFrameMessage, CAMERA_FRAME_MESSAGE } from 'common/felicia-proto';
import MESSAGE_TYPES from 'common/message-type';
import feliciaJs from 'felicia_js.node';
import TOPIC_MAP from 'topic-map';

export default function handleCameraMessage(topic, ws) {
  if (!TOPIC_MAP.has(topic)) {
    console.error(`topicMap doesn't hold requested ${topic}.`);
    return;
  }

  const topicInfo = TOPIC_MAP.get(topic);
  console.log(topicInfo);

  feliciaJs.MasterProxy.subscribeTopic(
    topicInfo,
    message => {
      switch (message.type) {
        case CAMERA_FRAME_MESSAGE: {
          const buffer = CameraFrameMessage.encode(message.message).finish();
          ws.broadcast(topic, buffer, MESSAGE_TYPES.Camera);
          break;
        }
        default: {
          console.error(
            `Don't know how to handle the message type ${message.type} for topic ${topic}`
          );
          break;
        }
      }
    },
    status => {
      console.log(`[TOPIC]: ${topic}`);
      console.error(status.errorMessage());
    },
    new feliciaJs.Settings({
      period: 100,
      queue_size: 1,
      isDynamicBuffer: true,
    })
  );
}
