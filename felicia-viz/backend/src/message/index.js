import MESSAGE_TYPES from 'common/message-type';

import handleMetaInfoMessage from 'message/meta-info-handler';
import handleCameraMessage, { handleCameraClose } from 'message/camera-handler';

export default function handleMessage(connection, ws, message) {
  /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["connection"] }] */
  let data;
  try {
    data = JSON.parse(message);
  } catch (e) {
    console.error(e);
    return;
  }

  const { type, topic } = data;
  switch (type) {
    case MESSAGE_TYPES.MetaInfo.name: {
      connection.type = MESSAGE_TYPES.MetaInfo;
      handleMetaInfoMessage(connection, data);
      break;
    }
    case MESSAGE_TYPES.Camera.name: {
      connection.type = MESSAGE_TYPES.Camera;
      connection.topic = topic;
      handleCameraMessage(topic, ws);
      break;
    }
    default: {
      console.error(`[OnMessage] Don't know how to handle the message type ${type}.`);
      break;
    }
  }
}

export function handleClose(connection) {
  const { type, topic } = connection;
  switch (type) {
    case MESSAGE_TYPES.Camera: {
      handleCameraClose(topic);
      break;
    }
    default: {
      console.error(`[OnClose] Don't know how to handle the message type ${type}.`);
      break;
    }
  }
}
