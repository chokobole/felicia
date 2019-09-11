import { SizeMessage } from './geometry';

export const CAMERA_FORMAT_MESSAGE = 'felicia.drivers.CameraFormatMessage';

export default class CameraFormatMessage {
  constructor(message) {
    const { size, pixelFormat, frameRate } = message;
    this.size = new SizeMessage(size);
    this.pixelFormat = pixelFormat;
    this.frameRate = frameRate;
  }
}
