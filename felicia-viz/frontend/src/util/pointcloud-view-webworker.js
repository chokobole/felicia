/* global self */
/* eslint no-restricted-globals: ["off"] */
import { PointReader, ColorReader } from 'util/data-message-reader';

self.onmessage = event => {
  let message = null;
  const { colors, positions, frame } = event.data;
  const pointsReader = new PointReader(frame.points);
  const colorsReader = new ColorReader(frame.colors);

  const size = positions.length / 3;
  for (let i = 0; i < size; i += 1) {
    const colorsIdx = i * 4;
    const positionsIdx = i * 3;
    if (pointsReader.hasData(i)) {
      [
        positions[positionsIdx],
        positions[positionsIdx + 1],
        positions[positionsIdx + 2],
      ] = pointsReader.nextPoint3(i);
      if (colorsReader.hasData(i)) {
        [
          colors[colorsIdx],
          colors[colorsIdx + 1],
          colors[colorsIdx + 2],
          colors[colorsIdx + 3],
        ] = colorsReader.nextColor4f(i);
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
