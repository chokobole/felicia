/* global self */
/* eslint no-restricted-globals: ["off"] */
import { getDataView } from '@felicia-viz/proto/messages/data-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
import CameraFrameMessage from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { fillPixels } from '../util/color';
import Histogram from '../util/histogram';

export interface InputEvent {
  data: {
    imageData: ImageData;
    frame: CameraFrameMessage | DepthCameraFrameMessage;
    filter?: string;
    frameToAlign: CameraFrameMessage | null;
  };
}

export interface OutputEvent {
  data: ImageData;
}

const worker: Worker = self as any;

let histogram: Histogram;

worker.onmessage = (event: InputEvent): void => {
  const { imageData, frame, filter, frameToAlign } = event.data;
  const pixels = imageData.data;
  const { converted, data, cameraFormat } = frame;
  const { size, pixelFormat } = cameraFormat;
  const { width, height } = size;
  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_Z16) {
    const { min, max } = frame as DepthCameraFrameMessage;
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

  worker.postMessage(imageData);
};
