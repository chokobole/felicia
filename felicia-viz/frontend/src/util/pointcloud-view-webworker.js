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
    const colorsdataIdx = positionsIdx * 4;
    if (pointsDataIdx + 8 < pointsData.byteLength) {
      positions[positionsIdx] = pointsData.getFloat32(pointsDataIdx, true);
      positions[positionsIdx + 1] = pointsData.getFloat32(pointsDataIdx + 4, true);
      positions[positionsIdx + 2] = pointsData.getFloat32(pointsDataIdx + 8, true);
      if (colorsdataIdx + 8 < colorsData.byteLength) {
        colors[colorsIdx] = colorsData.getFloat32(colorsdataIdx + 0, true);
        colors[colorsIdx + 1] = colorsData.getFloat32(colorsdataIdx + 4, true);
        colors[colorsIdx + 2] = colorsData.getFloat32(colorsdataIdx + 8, true);
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
