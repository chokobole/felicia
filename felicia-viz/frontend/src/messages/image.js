/* eslint import/prefer-default-export: "off" */
export class Image {
  constructor(message) {
    const { size, pixelFormat, data } = message;
    const { width, height } = size;
    this.data = data;
    this.pixelFormat = pixelFormat;
    this.width = width;
    this.height = height;
  }
}
