/* global self */
/* eslint no-restricted-globals: ["off"] */
import Histogram from 'util/histogram';

let histogram;

self.onmessage = event => {
  let message = null;
  switch (event.data.source) {
    case 'pointcloudView': {
      const {
        colors,
        positions,
        width,
        height,
        data,
        pixelFormat,
        scale,
        filter,
      } = event.data.data;
      switch (pixelFormat) {
        case 'PIXEL_FORMAT_Z16': {
          if (!histogram) {
            histogram = new Histogram();
          }

          const { buffer, byteOffset, byteLength } = data;
          const pixelData = new Uint16Array(buffer, byteOffset, byteLength / 2);
          histogram.make(pixelData, width, height);
          histogram.fillVerticesWithColormap(
            colors,
            positions,
            pixelData,
            width,
            height,
            scale,
            filter
          );
          break;
        }
        default:
          console.error(`Not implemented yet for this format: ${pixelFormat}`);
      }

      message = {
        colors,
        positions,
      };
      break;
    }
    default:
      break;
  }

  if (message) {
    self.postMessage(message);
  }
};
