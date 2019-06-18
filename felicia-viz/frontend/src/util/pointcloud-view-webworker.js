/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const { colors, positions, frame, filter, frameToAlign } = event.data;
  const { width, height, pixelFormat, data, min, max } = frame;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    if (!histogram) {
      histogram = new Histogram();
    }

    const { buffer, byteOffset, byteLength } = data;
    const pixelData = new Uint16Array(buffer, byteOffset, byteLength / 2);
    histogram.fillVerticesWithColormap(
      colors,
      positions,
      pixelData,
      width,
      height,
      min,
      max,
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
