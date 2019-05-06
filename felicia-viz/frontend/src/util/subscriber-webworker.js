/* global self */
/* eslint no-restricted-globals: ["off", "self"] */
import protobuf from 'protobufjs/light';
import feliciaProtobufJson from 'common/proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
const CameraFrameMessage = FeliciaProtoRoot.lookupType('felicia.CameraFrameMessage');

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'subscribeCamera': {
      const decoded = CameraFrameMessage.decode(new Uint8Array(data));
      message = CameraFrameMessage.toObject(decoded, { enums: String });
      message.type = 'Camera';
      break;
    }
    case 'subscribeGeneral': {
      message = JSON.parse(data);
      message.type = 'General';
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
