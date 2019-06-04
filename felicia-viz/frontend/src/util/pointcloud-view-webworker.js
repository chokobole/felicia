/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const {
    colors,
    positions,
    width,
    height,
    data,
    pixelFormat,
    scale,
    filter,
    frameToAlign,
  } = event.data;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    if (!histogram) {
      histogram = new Histogram();
    }

    const { buffer, byteOffset, byteLength } = data;
    const pixelData = new Uint16Array(buffer, byteOffset, byteLength / 2);
    histogram.make(pixelData, width, height);
    histogram.fillVerticesWithColormap(
      colors,
      positions,
      pixelData,
      width,
      height,
      scale,
      filter,
      frameToAlign
    );
  } else {
    console.error(`Not implemented yet for this format: ${pixelFormat}`);
    return;
  }

  message = {
    colors,
    positions,
  };

  if (message) {
    self.postMessage(message);
  }
};
