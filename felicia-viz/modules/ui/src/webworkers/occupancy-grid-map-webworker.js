/* global self */
/* eslint no-restricted-globals: ["off"] */
self.onmessage = event => {
  const { imageData, occupancyGridMap } = event.data;
  const { size, data } = occupancyGridMap;
  const { width, height } = size;

  for (let i = 0; i < height; i += 1) {
    for (let j = 0; j < width; j += 1) {
      const idx = i * width + j;
      const pixelsIdx = idx << 2;
      const pixels = imageData.data;
      const v = data[idx];
      if (v >= 0 && v <= 100) {
        const c = Math.round(255 * (1 - v / 100));
        pixels[pixelsIdx] = c;
        pixels[pixelsIdx + 1] = c;
        pixels[pixelsIdx + 2] = c;
        pixels[pixelsIdx + 3] = 255;
      } else {
        pixels[pixelsIdx + 3] = 0;
      }
    }
  }

  self.postMessage(imageData);
};
