/* global self */
/* eslint no-restricted-globals: ["off", "self"] */
import protobuf from 'protobufjs/light';

import feliciaProtobufJson from 'common/proto_bundle/felicia_proto_bundle.json';
import TYPES from 'common/connection-type';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
const CameraFrameMessage = FeliciaProtoRoot.lookupType('felicia.CameraFrameMessage');

self.onmessage = event => {
  let message = null;
  const { data, type, destinations } = event.data;
  switch (type) {
    case TYPES.Camera.name: {
      const decoded = CameraFrameMessage.decode(new Uint8Array(data));
      message = {
        data: CameraFrameMessage.toObject(decoded, { enums: String }),
        type: TYPES.Camera.name,
        destinations,
      };
      break;
    }
    case TYPES.General.name: {
      let parsed = null;
      try {
        parsed = JSON.parse(data);
      } catch (e) {
        console.error(e);
        break;
      }
      message = {
        data: parsed,
        type: TYPES.General.name,
        destinations,
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
