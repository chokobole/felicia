/* global self */
/* eslint no-restricted-globals: ["off"] */
import { ImageFormat } from '@felicia-viz/communication';

import { RGBA, RGB, BGRA, BGR, fillPixels } from 'util/color';

self.onmessage = event => {
  let message = null;
  const { imageData, frame } = event.data;
  const pixels = imageData.data;
  const { image, boundingBoxes } = frame;
  const { width, height, data, imageFormat } = image;
  if (imageFormat === ImageFormat.values.IMAGE_FORMAT_RGB) {
    fillPixels(pixels, width, height, data, RGB);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_RGBA) {
    fillPixels(pixels, width, height, data, RGBA);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_BGR) {
    fillPixels(pixels, width, height, data, BGR);
  } else if (imageFormat === ImageFormat.values.IMAGE_FORMAT_BGRA) {
    fillPixels(pixels, width, height, data, BGRA);
  } else {
    console.error(`Unknown format format: ${imageFormat}`);
    return;
  }

  message = {
    imageData,
    boundingBoxes,
  };

  if (message) {
    self.postMessage(message);
  }
};
