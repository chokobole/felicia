/* global self */
/* eslint no-restricted-globals: ["off"] */
import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'imageView': {
      const { imageData, width, height, pixelFormat, filter } = data;
      const pixels = imageData.data;
      switch (pixelFormat) {
        case 'PIXEL_FORMAT_Z16': {
          if (!histogram) {
            histogram = new Histogram();
          }

          const { buffer, byteOffset, byteLength } = data.data;
          const pixelData = new Uint16Array(buffer, byteOffset, byteLength / 2);
          histogram.make(pixelData, width, height);
          histogram.fillImageDataWithColormap(pixels, pixelData, width, height, filter);
          break;
        }
        case 'PIXEL_FORMAT_ARGB': {
          const pixelData = new Uint8ClampedArray(data.data);
          for (let i = 0; i < height; i += 1) {
            for (let j = 0; j < width; j += 1) {
              const index = 4 * width * i + j * 4;
              pixels[index] = pixelData[index + 2];
              pixels[index + 1] = pixelData[index + 1];
              pixels[index + 2] = pixelData[index];
              pixels[index + 3] = pixelData[index + 3];
            }
          }
          break;
        }
        default:
          console.error(`Not implemented yet for this format: ${pixelFormat}`);
      }

      message = imageData;
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
