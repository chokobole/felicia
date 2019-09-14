/* global self */
/* eslint no-restricted-globals: ["off"] */
import { fillPixels } from '../util/color';

self.onmessage = event => {
  let message = null;
  const { imageData, image, data } = event.data;
  const pixels = imageData.data;
  const { size, pixelFormat } = image;
  const { width, height } = size;
  if (!fillPixels(pixels, width, height, image.data, pixelFormat)) return;

  message = {
    imageData,
    data,
  };

  if (message) {
    self.postMessage(message);
  }
};
