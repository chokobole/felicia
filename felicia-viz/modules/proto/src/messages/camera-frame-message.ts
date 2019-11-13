// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import CameraFormatMessage, { CameraFormatMessageProtobuf } from './camera-format-message';

export const CAMERA_FRAME_MESSAGE = 'felicia.drivers.CameraFrameMessage';

export interface CameraFrameMessageProtobuf {
  converted: boolean;
  data: Uint8Array;
  cameraFormat: CameraFormatMessageProtobuf;
  timestamp: number;
}

export default class CameraFrameMessage {
  converted: boolean;

  data: Uint8Array;

  cameraFormat: CameraFormatMessage;

  timestamp: number;

  constructor({ converted, data, cameraFormat, timestamp }: CameraFrameMessageProtobuf) {
    this.converted = converted;
    this.data = data;
    this.cameraFormat = new CameraFormatMessage(cameraFormat);
    this.timestamp = timestamp;
  }
}
