import colormap from 'colormap';

export default class Histogram {
  histogram = null;

  skipIdx = 0;

  make(pixelData, width, height) {
    if (!this.histogram) {
      this.histogram = new Array(256).fill(0);
    } else {
      this.histogram.fill(0);
    }

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const pixelDataIndex = width * i + j;
        const k = Math.floor((pixelData[pixelDataIndex] + 1) / 256);
        this.histogram[k] += 1;
      }
    }
  }

  /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["pixels", "colors", "positions"] }] */
  fillImageDataWithColormap(pixels, pixelData, width, height, filter) {
    const cm = colormap({
      colormap: filter,
      nshades: 256,
      format: 'rgba',
      alpha: 1,
    });

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const pixelDataIndex = width * i + j;
        const index = 4 * pixelDataIndex;
        const k = Math.floor((pixelData[pixelDataIndex] + 1) / 256);
        const v = Math.floor(256 * (this.histogram[k] / (width * height)));
        const color = cm[v];
        [pixels[index], pixels[index + 1], pixels[index + 2]] = color;
        pixels[index + 3] = 255;
      }
    }
  }

  fillVerticesWithColormap(colors, positions, pixelData, width, height, scale, filter) {
    const cm = colormap({
      colormap: filter,
      nshades: 256,
      format: 'rgba',
      alpha: 1,
    });

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const pixelDataIndex = width * i + j;
        const positionIdx = 3 * pixelDataIndex;
        const colorIdx = 4 * pixelDataIndex;

        const k = Math.floor((pixelData[pixelDataIndex] + 1) / 256);
        const v = Math.floor(256 * (this.histogram[k] / (width * height)));
        const [r, g, b] = cm[v];

        positions[positionIdx + 2] = -pixelData[pixelDataIndex] * scale * 300;

        colors[colorIdx] = (r + 1) / 256;
        colors[colorIdx + 1] = (g + 1) / 256;
        colors[colorIdx + 2] = (b + 1) / 256;
      }
    }
  }
}
