/* global self */
/* eslint no-restricted-globals: ["off", "self"] */
import protobuf from 'protobufjs/light';

import feliciaProtobufJson from 'common/proto_bundle/felicia_proto_bundle.json';
import TYPES from 'common/connection-type';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
const CameraFrameMessage = FeliciaProtoRoot.lookupType('felicia.CameraFrameMessage');

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'subscribeCamera': {
      const decoded = CameraFrameMessage.decode(new Uint8Array(data));
      message = {
        data: CameraFrameMessage.toObject(decoded, { enums: String }),
        type: TYPES.Camera.name,
        id: event.data.id,
      };
      break;
    }
    case 'subscribeGeneral': {
      try {
        message = JSON.parse(data);
      } catch (e) {
        console.error(e);
        break;
      }
      message.type = TYPES.General.name;
      message.id = data.id;
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
