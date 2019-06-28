/* global self */
/* eslint no-restricted-globals: ["off"] */
/* eslint no-bitwise: ["off"] */
/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["pixels"] }] */
import { PixelFormat } from '@felicia-viz/communication';

import { RGBA, RGB, BGRA, BGR } from 'util/color';
import Histogram from 'util/histogram';
import { getDataView } from 'util/util';

let histogram;

function fillPixels(pixels, width, height, data, colorIndexes) {
  const pixelData = new Uint8ClampedArray(data);
  const size = width * height;
  if (colorIndexes.aIdx) {
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      pixels[pixelsIdx + RGBA.rIdx] = pixelData[pixelsIdx + colorIndexes.rIdx];
      pixels[pixelsIdx + RGBA.gIdx] = pixelData[pixelsIdx + colorIndexes.gIdx];
      pixels[pixelsIdx + RGBA.bIdx] = pixelData[pixelsIdx + colorIndexes.bIdx];
      pixels[pixelsIdx + RGBA.aIdx] = pixelData[pixelsIdx + colorIndexes.aIdx];
    }
  } else {
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      const pixelDataIdx = i * 3;
      pixels[pixelsIdx + RGBA.rIdx] = pixelData[pixelDataIdx + colorIndexes.rIdx];
      pixels[pixelsIdx + RGBA.gIdx] = pixelData[pixelDataIdx + colorIndexes.gIdx];
      pixels[pixelsIdx + RGBA.bIdx] = pixelData[pixelDataIdx + colorIndexes.bIdx];
      pixels[pixelsIdx + RGBA.aIdx] = 255;
    }
  }
}

self.onmessage = event => {
  let message = null;
  const { imageData, frame, filter, frameToAlign } = event.data;
  const pixels = imageData.data;
  const { pixelFormat, width, height, min, max, data, converted } = frame;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    if (!histogram) {
      histogram = new Histogram();
    }

    const pixelData = getDataView(data);
    histogram.fillImageDataWithColormap(
      pixels,
      pixelData,
      width,
      height,
      min,
      max,
      filter,
      frameToAlign
    );
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB || converted) {
    fillPixels(pixels, width, height, data, BGRA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGB24) {
    fillPixels(pixels, width, height, data, BGR);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGB32) {
    fillPixels(pixels, width, height, data, BGRA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ABGR) {
    fillPixels(pixels, width, height, data, RGBA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_XBGR) {
    fillPixels(pixels, width, height, data, RGB);
  } else {
    console.error(`To draw, you need to convert to ARGB format: ${pixelFormat}`);
    return;
  }

  message = imageData;

  if (message) {
    self.postMessage(message);
  }
};
