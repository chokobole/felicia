/* eslint no-bitwise: ["off"] */
/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["imageData", "colors", "dst", "positions"] }] */
import colormap from 'colormap';

import { PixelFormat } from '@felicia-viz/communication';

import { RGBA, BGRA } from 'util/color';

function align(dst, frameToAlign, width, height, normalize) {
  if (!frameToAlign) return;

  const { cameraFormat, data, converted } = frameToAlign.frame;
  const { pixelFormat } = cameraFormat;

  if (cameraFormat.width !== width || cameraFormat.height !== height) {
    console.error(
      `Resolution mismatched, Depth: (${width}, ${height}) Color: (${cameraFormat.width}, ${
        cameraFormat.height
      })`
    );
    return;
  }

  if (pixelFormat === PixelFormat.values.PIXEL_FORMAT_ARGB || converted) {
    const size = width * height;
    const pixelData = new Uint8ClampedArray(data);
    if (normalize) {
      for (let i = 0; i < size; i += 1) {
        const imageDataIdx = i << 2;
        dst[imageDataIdx + RGBA.rIdx] =
          (dst[imageDataIdx + RGBA.rIdx] + pixelData[imageDataIdx + BGRA.rIdx] / 256) / 2;
        dst[imageDataIdx + RGBA.gIdx] =
          (dst[imageDataIdx + RGBA.gIdx] + pixelData[imageDataIdx + BGRA.gIdx] / 256) / 2;
        dst[imageDataIdx + RGBA.bIdx] =
          (dst[imageDataIdx + RGBA.bIdx] + pixelData[imageDataIdx + BGRA.bIdx] / 256) / 2;
        dst[imageDataIdx + RGBA.aIdx] =
          (dst[imageDataIdx + RGBA.aIdx] + pixelData[imageDataIdx + BGRA.aIdx] / 256) / 2;
      }
    } else {
      for (let i = 0; i < size; i += 1) {
        const imageDataIdx = i << 2;
        dst[imageDataIdx + RGBA.rIdx] =
          (dst[imageDataIdx + RGBA.rIdx] + pixelData[imageDataIdx + BGRA.rIdx]) >> 1;
        dst[imageDataIdx + RGBA.gIdx] =
          (dst[imageDataIdx + RGBA.gIdx] + pixelData[imageDataIdx + BGRA.gIdx]) >> 1;
        dst[imageDataIdx + RGBA.bIdx] =
          (dst[imageDataIdx + RGBA.bIdx] + pixelData[imageDataIdx + BGRA.bIdx]) >> 1;
        dst[imageDataIdx + RGBA.aIdx] =
          (dst[imageDataIdx + RGBA.aIdx] + pixelData[imageDataIdx + BGRA.aIdx]) >> 1;
      }
    }
  } else {
    console.error(`Color format is not ARGB: ${pixelFormat}`);
  }
}

export default class Histogram {
  histogram = null;

  make(pixelData, width, height) {
    if (!this.histogram) {
      this.histogram = new Array(256).fill(0);
    } else {
      this.histogram.fill(0);
    }

    const size = width * height;
    for (let i = 0; i < size; i += 1) {
      const k = pixelData[i] >> 8;
      this.histogram[k] += 1;
    }

    for (let i = 0; i < 256; i += 1) {
      this.histogram[i] = Math.floor((this.histogram[i] / size) * 256);
    }
  }

  fillImageDataWithColormap(imageData, pixelData, width, height, filter, frameToAlign) {
    this.make(pixelData, width, height);

    const cm = colormap({
      colormap: filter,
      nshades: 256,
      format: 'rgba',
      alpha: 1,
    });

    const size = width * height;
    for (let i = 0; i < size; i += 1) {
      const imageDataIdx = i << 2;
      const k = pixelData[i] >> 8;
      const c = cm[this.histogram[k]];
      [imageData[imageDataIdx], imageData[imageDataIdx + 1], imageData[imageDataIdx + 2]] = c;
      imageData[imageDataIdx + 3] = 255;
    }

    align(imageData, frameToAlign, width, height, false);
  }

  fillVerticesWithColormap(
    colors,
    positions,
    pixelData,
    width,
    height,
    scale,
    filter,
    frameToAlign
  ) {
    this.make(pixelData, width, height);

    const cm = colormap({
      colormap: filter,
      nshades: 256,
      format: 'float',
      alpha: 1,
    });

    const size = width * height;
    for (let i = 0; i < size; i += 1) {
      const positionIdx = 3 * i;
      const colorIdx = i << 2;

      positions[positionIdx + 2] = -pixelData[i] * scale * 300;
      const k = pixelData[i] >> 8;
      const c = cm[this.histogram[k]];
      [colors[colorIdx], colors[colorIdx + 1], colors[colorIdx + 2]] = c;
    }

    align(colors, frameToAlign, width, height, true);
  }
}
