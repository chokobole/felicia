/* eslint import/prefer-default-export: "off" */
import { Color3, Color4 } from '@babylonjs/core/Maths/math';

import FeliciaProtoRoot from '../felicia-proto-root';
import { SizeMessage } from './geometry';

export const COLOR3U_MESSAGE = 'felicia.Color3uMessage';
export const COLOR3F_MESSAGE = 'felicia.Color3fMessage';
export const COLOR4U_MESSAGE = 'felicia.Color4uMessage';
export const COLOR4F_MESSAGE = 'felicia.Color4fMessage';
export const IMAGE_MESSAGE = 'felicia.ImageMessage';

export const PixelFormat = FeliciaProtoRoot.lookupEnum('felicia.PixelFormat');

export class Color3uMessage {
  constructor(message) {
    const { rgb } = message;
    this.r = (rgb >> 16) & 0xff;
    this.g = (rgb >> 8) & 0xff;
    this.b = rgb & 0xff;
  }

  toBabylonColor3() {
    const { r, g, b } = this;
    return new Color3(r / 255, g / 255, b / 255);
  }

  toBabylonColor4() {
    const { r, g, b } = this;
    return new Color3(r / 255, g / 255, b / 255, 1);
  }
}

export class Color3fMessage {
  constructor(message) {
    const { r, g, b } = message;
    this.r = r;
    this.g = g;
    this.b = b;
  }

  toBabylonColor3() {
    const { r, g, b } = this;
    return new Color3(r, g, b);
  }

  toBabylonColor4() {
    const { r, g, b } = this;
    return new Color4(r, g, b, 1);
  }
}

export class Color4uMessages {
  constructor(message) {
    const { rgba } = message;
    this.r = (rgba >> 24) & 0xff;
    this.g = (rgba >> 16) & 0xff;
    this.b = (rgba >> 8) & 0xff;
    this.a = rgba & 0xff;
  }

  toBabylonColor3() {
    const { r, g, b } = this;
    return new Color3(r / 255, g / 255, b / 255);
  }

  toBabylonColor4() {
    const { r, g, b, a } = this;
    return new Color4(r / 255, g / 255, b / 255, a / 255);
  }
}

export class Color4fMessages {
  constructor(message) {
    const { r, g, b, a } = message;
    this.r = r;
    this.g = g;
    this.b = b;
    this.a = a;
  }

  toBabylonColor3() {
    const { r, g, b } = this;
    return new Color3(r, g, b);
  }

  toBabylonColor4() {
    const { r, g, b, a } = this;
    return new Color4(r, g, b, a);
  }
}

export class Image {
  constructor(message) {
    const { size, pixelFormat, data } = message;
    this.size = new SizeMessage(size);
    this.pixelFormat = pixelFormat;
    this.data = data;
  }
}
