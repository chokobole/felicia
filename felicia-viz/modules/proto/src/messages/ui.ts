/* eslint import/prefer-default-export: "off" */
import { Color3, Color4 } from '@babylonjs/core/Maths/math';
import FeliciaProtoRoot from '../felicia-proto-root';
import { SizeMessage, SizeMessageProtobuf } from './geometry';

export const COLOR3U_MESSAGE = 'felicia.Color3uMessage';
export const COLOR3F_MESSAGE = 'felicia.Color3fMessage';
export const COLOR4U_MESSAGE = 'felicia.Color4uMessage';
export const COLOR4F_MESSAGE = 'felicia.Color4fMessage';
export const IMAGE_MESSAGE = 'felicia.ImageMessage';

export const PixelFormat = FeliciaProtoRoot.lookupEnum('felicia.PixelFormat');

export interface Color3uMessageProtobuf {
  rgb: number;
}

export class Color3uMessage {
  r: number;

  g: number;

  b: number;

  constructor({ rgb }: Color3uMessageProtobuf) {
    this.r = (rgb >> 16) & 0xff;
    this.g = (rgb >> 8) & 0xff;
    this.b = rgb & 0xff;
  }

  toBabylonColor3(): Color3 {
    const { r, g, b } = this;
    return new Color3(r / 255, g / 255, b / 255);
  }

  toBabylonColor4(): Color4 {
    const { r, g, b } = this;
    return new Color4(r / 255, g / 255, b / 255, 1);
  }
}

export interface Color3fMessageProtobuf {
  r: number;
  g: number;
  b: number;
}

export class Color3fMessage {
  r: number;

  g: number;

  b: number;

  constructor({ r, g, b }: Color3fMessageProtobuf) {
    this.r = r;
    this.g = g;
    this.b = b;
  }

  toBabylonColor3(): Color3 {
    const { r, g, b } = this;
    return new Color3(r, g, b);
  }

  toBabylonColor4(): Color4 {
    const { r, g, b } = this;
    return new Color4(r, g, b, 1);
  }
}

export interface Color4uMessageProtobuf {
  rgba: number;
}

export class Color4uMessages {
  r: number;

  g: number;

  b: number;

  a: number;

  constructor({ rgba }: Color4uMessageProtobuf) {
    this.r = (rgba >> 24) & 0xff;
    this.g = (rgba >> 16) & 0xff;
    this.b = (rgba >> 8) & 0xff;
    this.a = rgba & 0xff;
  }

  toBabylonColor3(): Color3 {
    const { r, g, b } = this;
    return new Color3(r / 255, g / 255, b / 255);
  }

  toBabylonColor4(): Color4 {
    const { r, g, b, a } = this;
    return new Color4(r / 255, g / 255, b / 255, a / 255);
  }
}

export interface Color4fMessageProtobuf {
  r: number;
  g: number;
  b: number;
  a: number;
}

export class Color4fMessages {
  r: number;

  g: number;

  b: number;

  a: number;

  constructor({ r, g, b, a }: Color4fMessageProtobuf) {
    this.r = r;
    this.g = g;
    this.b = b;
    this.a = a;
  }

  toBabylonColor3(): Color3 {
    const { r, g, b } = this;
    return new Color3(r, g, b);
  }

  toBabylonColor4(): Color4 {
    const { r, g, b, a } = this;
    return new Color4(r, g, b, a);
  }
}

export enum PixelFormatProtobuf {
  PIXEL_FORMAT_UNKNOWN = 0,
  PIXEL_FORMAT_I420 = 1,
  PIXEL_FORMAT_YV12 = 2,
  PIXEL_FORMAT_NV12 = 6,
  PIXEL_FORMAT_NV21 = 7,
  PIXEL_FORMAT_UYVY = 8,
  PIXEL_FORMAT_YUY2 = 9,
  PIXEL_FORMAT_BGRA = 10,
  PIXEL_FORMAT_BGR = 12,
  PIXEL_FORMAT_BGRX = 13,
  PIXEL_FORMAT_MJPEG = 14,
  PIXEL_FORMAT_Y8 = 25,
  PIXEL_FORMAT_Y16 = 26,
  PIXEL_FORMAT_RGBA = 27,
  PIXEL_FORMAT_RGBX = 28,
  PIXEL_FORMAT_RGB = 29,
  PIXEL_FORMAT_ARGB = 30,
  PIXEL_FORMAT_Z16 = 31,
}

export interface ImageProtobuf {
  size: SizeMessageProtobuf;
  pixelFormat: PixelFormatProtobuf;
  data: Uint8Array;
}

export class Image {
  size: SizeMessage;

  pixelFormat: PixelFormatProtobuf;

  data: Uint8Array;

  constructor({ size, pixelFormat, data }: ImageProtobuf) {
    this.size = new SizeMessage(size);
    this.pixelFormat = pixelFormat;
    this.data = data;
  }
}
