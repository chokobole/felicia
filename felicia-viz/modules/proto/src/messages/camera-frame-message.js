import CameraFormatMessage from './camera-format-message';

export const CAMERA_FRAME_MESSAGE = 'felicia.drivers.CameraFrameMessage';

export default class CameraFrameMessage {
  constructor(message) {
    const { converted, data, cameraFormat, timestamp } = message;
    this.converted = converted;
    this.data = data;
    this.cameraFormat = new CameraFormatMessage(cameraFormat);
    this.timestamp = timestamp;
  }
}
