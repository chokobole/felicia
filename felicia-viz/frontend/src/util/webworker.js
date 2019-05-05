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
    case 'imageView': {
      const { imageData, width, height } = data;

      const pixelData = new Uint8ClampedArray(data.data);

      const pixels = imageData.data;
      for (let i = 0; i < height; i += 1) {
        for (let j = 0; j < width; j += 1) {
          const index = 4 * width * i + j * 4;
          pixels[index] = pixelData[index + 2];
          pixels[index + 1] = pixelData[index + 1];
          pixels[index + 2] = pixelData[index];
          pixels[index + 3] = pixelData[index + 3];
        }
      }

      message = imageData;
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
