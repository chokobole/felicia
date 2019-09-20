/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["imageData", "colors", "pixels", "positions"] }] */
import CameraFrameMessage from '@felicia-viz/proto/messages/camera-frame-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
// @ts-ignore
import colormap from 'colormap';
import { ARGB, BGR, BGRA, BGRX, Color3Indexes, Color4Indexes, RGB, RGBA, RGBX } from './color';

function blendPixels(
  pixels: Uint8ClampedArray,
  width: number,
  height: number,
  data: Uint8Array,
  colorIndexes: Color3Indexes | Color4Indexes
): void {
  const pixelData = new Uint8ClampedArray(data);
  const size = width * height;
  if ((colorIndexes as Color4Indexes).aIdx !== undefined) {
    if ((colorIndexes as Color4Indexes).aIdx < 0) {
      for (let i = 0; i < size; i += 1) {
        const pixelsIdx = i << 2;
        const pixelDataIdx = pixelsIdx;
        pixels[pixelsIdx + RGBA.rIdx] =
          (pixels[pixelsIdx + RGBA.rIdx] + pixelData[pixelDataIdx + colorIndexes.rIdx]) >> 1;
        pixels[pixelsIdx + RGBA.gIdx] =
          (pixels[pixelsIdx + RGBA.gIdx] + pixelData[pixelDataIdx + colorIndexes.gIdx]) >> 1;
        pixels[pixelsIdx + RGBA.bIdx] =
          (pixels[pixelsIdx + RGBA.bIdx] + pixelData[pixelDataIdx + colorIndexes.bIdx]) >> 1;
      }
    } else {
      for (let i = 0; i < size; i += 1) {
        const pixelsIdx = i << 2;
        const pixelDataIdx = pixelsIdx;
        pixels[pixelsIdx + RGBA.rIdx] =
          (pixels[pixelsIdx + RGBA.rIdx] + pixelData[pixelDataIdx + colorIndexes.rIdx]) >> 1;
        pixels[pixelsIdx + RGBA.gIdx] =
          (pixels[pixelsIdx + RGBA.gIdx] + pixelData[pixelDataIdx + colorIndexes.gIdx]) >> 1;
        pixels[pixelsIdx + RGBA.bIdx] =
          (pixels[pixelsIdx + RGBA.bIdx] + pixelData[pixelDataIdx + colorIndexes.bIdx]) >> 1;
        pixels[pixelsIdx + RGBA.aIdx] =
          (pixels[pixelsIdx + RGBA.aIdx] +
            pixelData[pixelDataIdx + (colorIndexes as Color4Indexes).aIdx]) >>
          1;
      }
    }
  } else {
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      const pixelDataIdx = i * 3;
      pixels[pixelsIdx + RGBA.rIdx] =
        (pixels[pixelsIdx + RGBA.rIdx] + pixelData[pixelDataIdx + colorIndexes.rIdx]) >> 1;
      pixels[pixelsIdx + RGBA.gIdx] =
        (pixels[pixelsIdx + RGBA.gIdx] + pixelData[pixelDataIdx + colorIndexes.gIdx]) >> 1;
      pixels[pixelsIdx + RGBA.bIdx] =
        (pixels[pixelsIdx + RGBA.bIdx] + pixelData[pixelDataIdx + colorIndexes.bIdx]) >> 1;
    }
  }
}

function align(
  pixels: Uint8ClampedArray,
  frameToAlign: CameraFrameMessage | null,
  width: number,
  height: number
): void {
  if (!frameToAlign) return;

  const { data, converted, cameraFormat } = frameToAlign;
  const { size, pixelFormat } = cameraFormat;

  if (size.width !== width || size.height !== height) {
    console.error(
      `Resolution mismatched, Depth: (${width}, ${height}) Color: (${size.width}, ${size.height})`
    );
    return;
  }

  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGRA || converted) {
    blendPixels(pixels, width, height, data, BGRA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGR) {
    blendPixels(pixels, width, height, data, BGR);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGRX) {
    blendPixels(pixels, width, height, data, BGRX);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGBA) {
    blendPixels(pixels, width, height, data, RGBX);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGBX) {
    blendPixels(pixels, width, height, data, RGBA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGB) {
    blendPixels(pixels, width, height, data, RGB);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB) {
    blendPixels(pixels, width, height, data, ARGB);
  } else {
    console.error(`To draw, you need to convert to BGRA format: ${pixelFormat}`);
  }
}

export default class Histogram {
  private histogram?: Array<number>;

  private make(pixelData: DataView, width: number, height: number, min: number, max: number): void {
    if (!this.histogram) {
      this.histogram = new Array(256).fill(0);
    } else {
      this.histogram.fill(0);
    }

    const size = width * height;
    let validSize = 0;
    for (let i = 1; i < size; i += 1) {
      const pixelDataIdx = i << 1;
      const v = pixelData.getUint16(pixelDataIdx, true);
      if (v >= min && v <= max) {
        const k = v >> 8;
        this.histogram[k] += 1;
        validSize += 1;
      }
    }

    let sum = 0;
    for (let i = 0; i < 256; i += 1) {
      sum += this.histogram[i];
      this.histogram[i] = Math.round((sum / validSize) * 255);
    }
  }

  fillImageDataWithColormap(
    imageData: Uint8ClampedArray,
    pixelData: DataView,
    width: number,
    height: number,
    min: number,
    max: number,
    filter: string | undefined,
    frameToAlign: CameraFrameMessage | null
  ): void {
    this.make(pixelData, width, height, min, max);

    const cm: Array<number[]> = colormap({
      colormap: filter,
      nshades: 256,
      format: 'rgba',
      alpha: 1,
    });

    const size = width * height;
    for (let i = 0; i < size; i += 1) {
      const pixelDataIdx = i << 1;
      const imageDataIdx = i << 2;
      const v = pixelData.getUint16(pixelDataIdx, true);
      if (v >= min && v <= max) {
        const k = v >> 8;
        const c = cm[this.histogram![k]];
        [imageData[imageDataIdx], imageData[imageDataIdx + 1], imageData[imageDataIdx + 2]] = c;
        imageData[imageDataIdx + 3] = 255;
      } else {
        imageData[imageDataIdx + 3] = 0;
      }
    }

    align(imageData, frameToAlign, width, height);
  }
}
