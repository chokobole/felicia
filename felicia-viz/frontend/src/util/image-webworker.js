/* global self */
/* eslint no-restricted-globals: ["off"] */
import { ImageFormat } from '@felicia-viz/communication';

import { RGBA, RGB, BGRA, BGR, fillPixels } from 'util/color';

self.onmessage = event => {
  let message = null;
  const { imageData, image, data } = event.data;
  const pixels = imageData.data;
  const { width, height, imageFormat } = image;
  if (imageFormat === ImageFormat.values.IMAGE_FORMAT_RGB) {
    fillPixels(pixels, width, height, image.data, RGB);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_RGBA) {
    fillPixels(pixels, width, height, image.data, RGBA);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_BGR) {
    fillPixels(pixels, width, height, image.data, BGR);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_BGRA) {
    fillPixels(pixels, width, height, image.data, BGRA);
  } else {
    console.error(`Unknown format format: ${imageFormat}`);
    return;
  }

  message = {
    imageData,
    data,
  };

  if (message) {
    self.postMessage(message);
  }
};
