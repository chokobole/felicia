/* global self */
/* eslint no-restricted-globals: ["off"] */
import MESSAGE_TYPES from 'common/message-type';
import { CameraFrameMessage } from 'common/felicia-proto';

self.onmessage = event => {
  let message = null;
  const { data, type, destinations } = event.data;
  switch (type) {
    case MESSAGE_TYPES.Camera.name: {
      const decoded = CameraFrameMessage.decode(new Uint8Array(data));
      message = {
        data: CameraFrameMessage.toObject(decoded, { enums: String }),
        type: MESSAGE_TYPES.Camera.name,
        destinations,
      };
      break;
    }
    case MESSAGE_TYPES.MetaInfo.name: {
      let parsed = null;
      try {
        parsed = JSON.parse(data);
      } catch (e) {
        console.error(e);
        break;
      }
      const { queryType } = parsed;
      message = {
        data: parsed.data,
        queryType,
        type: MESSAGE_TYPES.MetaInfo.name,
      };
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
