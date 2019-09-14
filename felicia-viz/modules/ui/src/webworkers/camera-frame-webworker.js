/* global self */
/* eslint no-restricted-globals: ["off"] */
import { getDataView } from '@felicia-viz/proto/messages/data-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';

import { fillPixels } from '../util/color';
import Histogram from '../util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const { imageData, frame, filter, frameToAlign } = event.data;
  const pixels = imageData.data;
  const { converted, data, cameraFormat } = frame;
  const { size, pixelFormat } = cameraFormat;
  const { width, height } = size;
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
