/* global self */
/* eslint no-restricted-globals: ["off"] */
function getFloat32Array(data) {
  const { buffer, byteOffset, byteLength } = data;
  const remainder = byteOffset % 4;
  if (remainder === 0) {
    return new Float32Array(buffer, byteOffset, byteLength / 4);
  }
  return new Float32Array(buffer, byteOffset + (4 - remainder), byteLength / 4 - 1);
}

self.onmessage = event => {
  let message = null;
  const { colors, positions, frame } = event.data;
  const pointsData = getFloat32Array(frame.points);
  const colorsData = getFloat32Array(frame.colors);

  const size = positions.length / 3;
  for (let i = 0; i < size; i += 1) {
    const colorsIdx = i * 4;
    const positionsIdx = i * 3;
    if (positionsIdx + 2 < pointsData.length) {
      positions[positionsIdx] = pointsData[positionsIdx];
      positions[positionsIdx + 1] = -pointsData[positionsIdx + 1];
      positions[positionsIdx + 2] = pointsData[positionsIdx + 2];
      if (positionsIdx + 2 < colorsData.length) {
        colors[colorsIdx] = colorsData[positionsIdx];
        colors[colorsIdx + 1] = colorsData[positionsIdx + 1];
        colors[colorsIdx + 2] = colorsData[positionsIdx + 2];
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
