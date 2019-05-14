/* global self */
/* eslint no-restricted-globals: ["off"] */
import colormap from 'colormap';

let histogram;

function makeHistogram(pixelData, width, height) {
  if (!histogram) {
    histogram = new Array(256).fill(0);
  } else {
    histogram.fill(0);
  }

  for (let i = 0; i < height; i += 1) {
    for (let j = 0; j < width; j += 1) {
      const pixelDataIndex = width * i + j;
      const k = Math.floor(pixelData[pixelDataIndex] / 256);
      histogram[k] += 1;
    }
  }
}

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'imageView': {
      const { imageData, width, height, pixelFormat, filter } = data;
      const pixels = imageData.data;
      switch (pixelFormat) {
        case 'PIXEL_FORMAT_Z16': {
          const colors = colormap({
            colormap: filter,
            nshades: 256,
            format: 'rgba',
            alpha: 1,
          });

          const pixelData = new Uint16Array(data.data.buffer);
          makeHistogram(pixelData, width, height);
          for (let i = 0; i < height; i += 1) {
            for (let j = 0; j < width; j += 1) {
              const index = 4 * width * i + j * 4;
              const pixelDataIndex = width * i + j;
              const k = Math.floor(pixelData[pixelDataIndex] / 256);
              const v = Math.floor(256 * (histogram[k] / (width * height)));
              const color = colors[v];
              [pixels[index], pixels[index + 1], pixels[index + 2]] = color;
              pixels[index + 3] = 255;
            }
          }
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
