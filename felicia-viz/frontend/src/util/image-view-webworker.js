/* global self */
/* eslint no-restricted-globals: ["off"] */
/* eslint no-bitwise: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import { RGBA, BGRA } from 'util/color';
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
    histogram.fillImageDataWithColormap(pixels, pixelData, width, height, filter, frameToAlign);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB || converted) {
    const pixelData = new Uint8ClampedArray(data);
    const size = width * height;
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      pixels[pixelsIdx + RGBA.rIdx] = pixelData[pixelsIdx + BGRA.rIdx];
      pixels[pixelsIdx + RGBA.gIdx] = pixelData[pixelsIdx + BGRA.gIdx];
      pixels[pixelsIdx + RGBA.bIdx] = pixelData[pixelsIdx + BGRA.bIdx];
      pixels[pixelsIdx + RGBA.aIdx] = pixelData[pixelsIdx + BGRA.aIdx];
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
