/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PixelFormat } from '@felicia-viz/communication/proto-types';

import { getDataView } from 'messages/data';
import { fillPixels } from 'util/color';
import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const { imageData, frame, filter, frameToAlign } = event.data;
  const pixels = imageData.data;
  const { converted, data, width, height, pixelFormat } = frame;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    const { min, max } = frame;
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
