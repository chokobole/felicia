// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* global self */
/* eslint no-restricted-globals: ["off"] */
import { OccupancyGridMapMessage } from '@felicia-viz/proto/messages/map-message';

export interface InputEvent {
  data: {
    imageData: ImageData;
    occupancyGridMap: OccupancyGridMapMessage;
  };
}

export interface OutputEvent {
  data: ImageData;
}

const worker: Worker = self as any;

worker.onmessage = (event: InputEvent): void => {
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

  worker.postMessage(imageData);
};
