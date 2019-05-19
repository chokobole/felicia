/* global self */
/* eslint no-restricted-globals: ["off"] */
import PROTO_TYPES, { TOPIC_INFO } from './proto-types';

self.onmessage = event => {
  let message = null;
  const { data, type, destinations } = event.data;
  if (type === TOPIC_INFO) {
    let parsed = null;
    try {
      parsed = JSON.parse(data);
    } catch (e) {
      console.error(e);
      return;
    }

    message = {
      data: parsed.data,
      type,
    };
  } else {
    const protoType = PROTO_TYPES[type];
    if (!protoType) {
      console.error(`Don't know how to handle the message type ${type}`);
      return;
    }

    const decoded = protoType.decode(new Uint8Array(data));
    message = {
      data: protoType.toObject(decoded, { enums: String }),
      type,
      destinations,
    };
  }

  if (message) {
    self.postMessage(message);
  }
};
