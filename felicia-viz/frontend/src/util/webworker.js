/* global self */
/* eslint no-restricted-globals: ["off", "self"] */

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'subscriber': {
      message = JSON.parse(data);
      break;
    }
    case 'imageView': {
      const { imageData, frame } = data;
      const { width, height } = frame;

      const pixelData = new Uint8ClampedArray(frame.data);

      const pixels = imageData.data;
      for (let i = 0; i < height; i += 1) {
        for (let j = 0; j < width; j += 1) {
          const index = 4 * width * i + j * 4;
          pixels[index] = pixelData[index + 2];
          pixels[index + 1] = pixelData[index + 1];
          pixels[index + 2] = pixelData[index];
          pixels[index + 3] = pixelData[index + 3];
        }
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
