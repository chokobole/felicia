// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* global self */
/* eslint no-restricted-globals: ["off"] */
import { Image } from '@felicia-viz/proto//messages/ui';
import { fillPixels } from '../util/color';

export interface InputEvent<T> {
  data: {
    imageData: ImageData;
    image: Image;
    data: T;
  };
}

export interface OutputEvent<T> {
  data: {
    imageData: ImageData;
    data: T;
  };
}

const worker: Worker = self as any;

worker.onmessage = (event: InputEvent<any>): void => {
  const { imageData, image, data } = event.data;
  const pixels = imageData.data;
  const { size, pixelFormat } = image;
  const { width, height } = size;
  if (!fillPixels(pixels, width, height, image.data, pixelFormat)) return;

  worker.postMessage({
    imageData,
    data,
  });
};
