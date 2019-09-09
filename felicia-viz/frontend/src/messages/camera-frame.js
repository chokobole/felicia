/* eslint import/prefer-default-export: "off" */
export class CameraFrameMessage {
  constructor(message) {
    const { converted, data, cameraFormat, timestamp } = message;
    const { size, pixelFormat, frameRate } = cameraFormat;
    const { width, height } = size;
    this.converted = converted;
    this.data = data;
    this.width = width;
    this.height = height;
    this.pixelFormat = pixelFormat;
    this.frameRate = frameRate;
    this.timestamp = timestamp;
  }
}
