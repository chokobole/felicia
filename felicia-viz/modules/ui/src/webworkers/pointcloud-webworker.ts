// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* global self */
/* eslint no-restricted-globals: ["off"] */
import { FloatArray } from '@babylonjs/core';
import { Colors, Points } from '@felicia-viz/proto/messages/data-message';
import { PointcloudMessage } from '@felicia-viz/proto/messages/map-message';

export interface MeshInfo {
  id: number;
  colors: FloatArray;
  positions: FloatArray;
}

export interface InputEvent {
  data: {
    meshInfos: Array<MeshInfo>;
    frame: PointcloudMessage;
  };
}

export interface OutputEvent {
  data: Array<MeshInfo>;
}

const worker: Worker = self as any;

worker.onmessage = (event: InputEvent): void => {
  const { meshInfos, frame } = event.data;
  const fPoints = new Points(frame.points);
  let fColors = frame.colors.data.byteLength > 0 ? new Colors(frame.colors) : null;

  let acc = 0;
  for (let i = 0; i < meshInfos.length; i += 1) {
    const { positions, colors } = meshInfos[i];
    const size = positions.length / 3;
    if (fColors) {
      for (let j = 0, k = acc; j < size; j += 1, k += 1) {
        const positionsIdx = j * 3;
        const colorsIdx = j * 4;
        if (fPoints.hasData(k)) {
          [
            positions[positionsIdx],
            positions[positionsIdx + 1],
            positions[positionsIdx + 2],
          ] = fPoints.nextPoint3(k);
          if (fColors.hasData(k)) {
            [
              colors[colorsIdx],
              colors[colorsIdx + 1],
              colors[colorsIdx + 2],
              colors[colorsIdx + 3],
            ] = fColors.nextColor4f(k);
          }
        } else {
          positions[positionsIdx + 2] = -1000000;
        }
      }
    } else {
      for (let j = 0, k = acc; j < size; j += 1, k += 1) {
        const positionsIdx = j * 3;
        if (fPoints.hasData(k)) {
          [
            positions[positionsIdx],
            positions[positionsIdx + 1],
            positions[positionsIdx + 2],
          ] = fPoints.nextPoint3(k);
        } else {
          positions[positionsIdx + 2] = -1000000;
        }
      }
    }
    acc += size;
  }

  worker.postMessage(meshInfos);
};
