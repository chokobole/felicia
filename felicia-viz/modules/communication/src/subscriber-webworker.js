/* global self */
/* eslint no-restricted-globals: ["off"] */
import Module from 'wasm/felicia_wasm.js'; // eslint-disable-line
import PROTO_TYPES, { PixelFormat, CAMERA_FRAME_MESSAGE, TOPIC_INFO } from './proto-types';

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
    const obj = protoType.toObject(decoded);

    if (type === CAMERA_FRAME_MESSAGE) {
      if (!Module.calledRun) return;

      const { cameraFormat } = obj;
      const { width, height, pixelFormat } = cameraFormat;
      const imgSize = obj.data.byteLength;
      const argbSize = width * height * 4;

      const imgBuffer = Module.createBuffer(imgSize);
      const argbBuffer = Module.createBuffer(argbSize);

      Module.HEAPU8.set(obj.data, imgBuffer);
      if (
        Module.convertToARGB(
          imgBuffer,
          imgSize,
          argbBuffer,
          width,
          height,
          PixelFormat.valuesById[pixelFormat]
        )
      ) {
        obj.data = new Uint8Array(Module.HEAPU8.buffer, argbBuffer, argbSize);
        obj.converted = true;
      }

      Module.releaseBuffer(imgBuffer);
      Module.releaseBuffer(argbBuffer);
    }

    message = {
      data: obj,
      type,
      destinations,
    };
  }

  if (message) {
    self.postMessage(message);
  }
};
