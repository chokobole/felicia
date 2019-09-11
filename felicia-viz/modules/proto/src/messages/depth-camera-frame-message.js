import CameraFrameMessage from './camera-frame-message';

export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.drivers.DepthCameraFrameMessage';

export default class DepthCameraFrameMessage extends CameraFrameMessage {
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
