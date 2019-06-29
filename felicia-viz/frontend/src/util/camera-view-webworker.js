/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import { RGBA, RGB, BGRA, BGR, fillPixels } from 'util/color';
import Histogram from 'util/histogram';
import { getDataView } from 'util/util';

let histogram;

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
