/* eslint no-bitwise: ["off"] */
/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["imageData", "colors", "pixels", "positions"] }] */
import colormap from 'colormap';

import { PixelFormat } from '@felicia-viz/communication';

import { RGBA, RGB, BGRA, BGRX, BGR, ARGB } from 'util/color';

function blendPixels(pixels, width, height, data, colorIndexes) {
  const pixelData = new Uint8ClampedArray(data);
  const size = width * height;
  if (colorIndexes.aIdx !== undefined) {
    if (colorIndexes.aIdx < 0) {
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
          (pixels[pixelsIdx + RGBA.aIdx] + pixelData[pixelDataIdx + colorIndexes.aIdx]) >> 1;
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

function align(pixels, frameToAlign, width, height) {
  if (!frameToAlign) return;

  const { pixelFormat, data, converted } = frameToAlign;

  if (frameToAlign.width !== width || frameToAlign.height !== height) {
    console.error(
      `Resolution mismatched, Depth: (${width}, ${height}) Color: (${frameToAlign.width}, ${
        frameToAlign.height
      })`
    );
    return;
  }

  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGRA || converted) {
    blendPixels(pixels, width, height, data, BGRA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGR) {
    blendPixels(pixels, width, height, data, BGR);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB) {
    blendPixels(pixels, width, height, data, ARGB);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGBA) {
    blendPixels(pixels, width, height, data, RGBA);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_RGB) {
    blendPixels(pixels, width, height, data, RGB);
  } else if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_BGRX) {
    blendPixels(pixels, width, height, data, BGRX);
  } else {
    console.error(`To draw, you need to convert to BGRA format: ${pixelFormat}`);
  }
}

export default class Histogram {
  make(pixelData, width, height, min, max) {
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
      this.histogram[i] = Math.floor((sum / validSize) * 255);
    }
  }

  fillImageDataWithColormap(imageData, pixelData, width, height, min, max, filter, frameToAlign) {
    this.make(pixelData, width, height, min, max);

    const cm = colormap({
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
        const c = cm[this.histogram[k]];
        [imageData[imageDataIdx], imageData[imageDataIdx + 1], imageData[imageDataIdx + 2]] = c;
        imageData[imageDataIdx + 3] = 255;
      } else {
        imageData[imageDataIdx + 3] = 0;
      }
    }

    align(imageData, frameToAlign, width, height);
  }
}
