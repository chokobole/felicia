/* global self */
/* eslint no-restricted-globals: ["off"] */
import { getDataView } from 'util/util';

self.onmessage = event => {
  let message = null;
  const { colors, positions, frame } = event.data;
  const pointsData = getDataView(frame.points);
  const colorsData = getDataView(frame.colors);

  const size = positions.length / 3;
  for (let i = 0; i < size; i += 1) {
    const colorsIdx = i * 4;
    const positionsIdx = i * 3;
    const pointsDataIdx = positionsIdx * 4;
    const colorsdataIdx = positionsIdx;
    if (pointsDataIdx + 8 < pointsData.byteLength) {
      positions[positionsIdx] = pointsData.getFloat32(pointsDataIdx, true);
      positions[positionsIdx + 1] = pointsData.getFloat32(pointsDataIdx + 4, true);
      positions[positionsIdx + 2] = pointsData.getFloat32(pointsDataIdx + 8, true);
      if (colorsdataIdx + 2 < colorsData.byteLength) {
        colors[colorsIdx] = colorsData.getUint8(colorsdataIdx + 0, true) / 255;
        colors[colorsIdx + 1] = colorsData.getUint8(colorsdataIdx + 1, true) / 255;
        colors[colorsIdx + 2] = colorsData.getUint8(colorsdataIdx + 2, true) / 255;
        colors[colorsIdx + 3] = 1;
      }
    } else {
      positions[positionsIdx + 2] = -1000000;
    }
  }

  message = {
    colors,
    positions,
  };

  if (message) {
    self.postMessage(message);
  }
};
