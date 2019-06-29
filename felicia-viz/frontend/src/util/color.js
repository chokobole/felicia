/* eslint import/prefer-default-export: "off" */
/* eslint no-bitwise: ["off"] */
/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["pixels"] }] */
export const RGBA = {
  rIdx: 0,
  gIdx: 1,
  bIdx: 2,
  aIdx: 3,
};
Object.freeze(RGBA);

export const RGB = {
  rIdx: 0,
  gIdx: 1,
  bIdx: 2,
};
Object.freeze(RGB);

export const BGRA = {
  rIdx: 2,
  gIdx: 1,
  bIdx: 0,
  aIdx: 3,
};
Object.freeze(BGRA);

export const BGR = {
  rIdx: 2,
  gIdx: 1,
  bIdx: 0,
};
Object.freeze(BGR);

export function fillPixels(pixels, width, height, data, colorIndexes) {
  const pixelData = new Uint8ClampedArray(data);
  const size = width * height;
  if (colorIndexes.aIdx) {
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      pixels[pixelsIdx + RGBA.rIdx] = pixelData[pixelsIdx + colorIndexes.rIdx];
      pixels[pixelsIdx + RGBA.gIdx] = pixelData[pixelsIdx + colorIndexes.gIdx];
      pixels[pixelsIdx + RGBA.bIdx] = pixelData[pixelsIdx + colorIndexes.bIdx];
      pixels[pixelsIdx + RGBA.aIdx] = pixelData[pixelsIdx + colorIndexes.aIdx];
    }
  } else {
    for (let i = 0; i < size; i += 1) {
      const pixelsIdx = i << 2;
      const pixelDataIdx = i * 3;
      pixels[pixelsIdx + RGBA.rIdx] = pixelData[pixelDataIdx + colorIndexes.rIdx];
      pixels[pixelsIdx + RGBA.gIdx] = pixelData[pixelDataIdx + colorIndexes.gIdx];
      pixels[pixelsIdx + RGBA.bIdx] = pixelData[pixelDataIdx + colorIndexes.bIdx];
      pixels[pixelsIdx + RGBA.aIdx] = 255;
    }
  }
}
