// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import CameraFrameMessage, { CameraFrameMessageProtobuf } from './camera-frame-message';

export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.drivers.DepthCameraFrameMessage';

export interface DepthCameraFrameMessageProtobuf extends CameraFrameMessageProtobuf {
  min: number;
  max: number;
}

export default class DepthCameraFrameMessage extends CameraFrameMessage {
  min: number;

  max: number;

  constructor({
    converted,
    data,
    cameraFormat,
    timestamp,
    min,
    max,
  }: DepthCameraFrameMessageProtobuf) {
    super({
      converted,
      data,
      cameraFormat,
      timestamp,
    });
    this.min = min;
    this.max = max;
  }
}
