/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const {
    imageData,
    width,
    height,
    data,
    pixelFormat,
    converted,
    filter,
    frameToAlign,
  } = event.data;
  const pixels = imageData.data;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    if (!histogram) {
      histogram = new Histogram();
    }

    const { buffer, byteOffset, byteLength } = data;
    const pixelData = new Uint16Array(buffer, byteOffset, byteLength / 2);
    histogram.make(pixelData, width, height);
    histogram.fillImageDataWithColormap(pixels, pixelData, width, height, filter, frameToAlign);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB || converted) {
    const pixelData = new Uint8ClampedArray(data);
    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const index = 4 * width * i + j * 4;
        pixels[index] = pixelData[index + 2];
        pixels[index + 1] = pixelData[index + 1];
        pixels[index + 2] = pixelData[index];
        pixels[index + 3] = pixelData[index + 3];
      }
    }
  } else {
    console.error(`To draw, you need to convert to ARGB format: ${pixelFormat}`);
    return;
  }

  message = imageData;

  if (message) {
    self.postMessage(message);
  }
};
