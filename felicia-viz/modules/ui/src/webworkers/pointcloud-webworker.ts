/* global self */
/* eslint no-restricted-globals: ["off"] */
import { FloatArray } from '@babylonjs/core';
import { Colors, Points } from '@felicia-viz/proto/messages/data-message';
import { PointcloudMessage } from '@felicia-viz/proto/messages/map-message';

export interface InputEvent {
  data: {
    colors: FloatArray;
    positions: FloatArray;
    frame: PointcloudMessage;
  };
}

export interface OutputEvent {
  data: {
    colors: FloatArray;
    positions: FloatArray;
  };
}

const worker: Worker = self as any;

worker.onmessage = (event: InputEvent): void => {
  const { colors, positions, frame } = event.data;

  const fPoints = new Points(frame.points);
  const fColors = new Colors(frame.colors);

  const size = positions.length / 3;
  if (fColors.length() > 0) {
    for (let i = 0; i < size; i += 1) {
      const colorsIdx = i * 4;
      const positionsIdx = i * 3;
      if (fPoints.hasData(i)) {
        [
          positions[positionsIdx],
          positions[positionsIdx + 1],
          positions[positionsIdx + 2],
        ] = fPoints.nextPoint3(i);
        if (fColors.hasData(i)) {
          [
            colors[colorsIdx],
            colors[colorsIdx + 1],
            colors[colorsIdx + 2],
            colors[colorsIdx + 3],
          ] = fColors.nextColor4f(i);
        }
      } else {
        positions[positionsIdx + 2] = -1000000;
      }
    }
  } else {
    for (let i = 0; i < size; i += 1) {
      const positionsIdx = i * 3;
      if (fPoints.hasData(i)) {
        [
          positions[positionsIdx],
          positions[positionsIdx + 1],
          positions[positionsIdx + 2],
        ] = fPoints.nextPoint3(i);
      } else {
        positions[positionsIdx + 2] = -1000000;
      }
    }
  }

  worker.postMessage({
    colors,
    positions,
  });
};
