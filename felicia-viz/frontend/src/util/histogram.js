import colormap from 'colormap';

export default class Histogram {
  histogram = null;

  make(pixelData, width, height) {
    if (!this.histogram) {
      this.histogram = new Array(256).fill(0);
    } else {
      this.histogram.fill(0);
    }

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const pixelDataIndex = width * i + j;
        const k = Math.floor(pixelData[pixelDataIndex] / 256);
        this.histogram[k] += 1;
      }
    }
  }

  fillImageDataWithColormap(pixels, pixelData, width, height, filter) {
    /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["pixels"] }] */
    const colors = colormap({
      colormap: filter,
      nshades: 256,
      format: 'rgba',
      alpha: 1,
    });

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const index = 4 * width * i + j * 4;
        const pixelDataIndex = width * i + j;
        const k = Math.floor(pixelData[pixelDataIndex] / 256);
        const v = Math.floor(256 * (this.histogram[k] / (width * height)));
        const color = colors[v];
        [pixels[index], pixels[index + 1], pixels[index + 2]] = color;
        pixels[index + 3] = 255;
      }
    }
  }
}
