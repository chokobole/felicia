/* global self */
/* eslint no-restricted-globals: ["off", "self"] */

self.onmessage = event => {
  let message = null;
  const { data } = event.data;
  switch (event.data.source) {
    case 'camera': {
      const parsed = JSON.parse(data);
      message = {
        currentTime: parseInt(parsed.currentTime, 10),
        frame: parsed.frame,
      };
      break;
    }
    case 'imageView': {
      const { imageData, frame } = data;
      const { width, height } = frame;

      const pixels = imageData.data;
      for (let i = 0; i < height; i += 1) {
        for (let j = 0; j < width; j += 1) {
          const index = 4 * width * i + j * 4;
          pixels[index] = frame.data[index + 2];
          pixels[index + 1] = frame.data[index + 1];
          pixels[index + 2] = frame.data[index];
          pixels[index + 3] = frame.data[index + 3];
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
