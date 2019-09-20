import CameraFrameMessage, { CameraFrameMessageProtobuf } from './camera-frame-message';

export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.drivers.DepthCameraFrameMessage';

export interface DepthCameraFrameMessageProtobuf extends CameraFrameMessageProtobuf {
  min: number;
  max: number;
}

export default class DepthCameraFrameMessage extends CameraFrameMessage {
  min: number;

  max: number;

  constructor({
    converted,
    data,
    cameraFormat,
    timestamp,
    min,
    max,
  }: DepthCameraFrameMessageProtobuf) {
    super({
      converted,
      data,
      cameraFormat,
      timestamp,
    });
    this.min = min;
    this.max = max;
  }
}
