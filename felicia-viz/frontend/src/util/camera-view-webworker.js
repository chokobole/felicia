/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication';

import { fillPixels } from 'util/color';
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
  } else if (
    !fillPixels(
      pixels,
      width,
      height,
      data,
      converted ? PixelFormat.values.PIXEL_FORMAT_BGRA : pixelFormat
    )
  ) {
    return;
  }

  message = imageData;

  if (message) {
    self.postMessage(message);
  }
};
