/* eslint import/prefer-default-export: "off" */
import { CameraFrameMessage } from './camera-frame';

export class DepthCameraFrameMessage extends CameraFrameMessage {
  constructor(message) {
    const { data, cameraFormat, timestamp, min, max } = message;
    super({
      data,
      cameraFormat,
      timestamp,
    });
    this.min = min;
    this.max = max;
  }
}
