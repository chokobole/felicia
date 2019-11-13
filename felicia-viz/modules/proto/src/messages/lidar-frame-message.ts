// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export const LIDAR_FRAME_MESSAGE = 'felicia.drivers.LidarFrameMessage';

export interface LidarFrameMessageProtobuf {
  angleStart: number;
  angleEnd: number;
  angleDelta: number;
  timeDelta: number;
  scanTime: number;
  rangeMin: number;
  rangeMax: number;
  ranges: Uint8Array;
  intensities: Uint8Array;
  timestamp: number;
}

export default class LidarFrameMessage {
  angleStart: number;

  angleEnd: number;

  angleDelta: number;

  timeDelta: number;

  scanTime: number;

  rangeMin: number;

  rangeMax: number;

  ranges: Uint8Array;

  intensities: Uint8Array;

  timestamp: number;

  constructor({
    angleStart,
    angleEnd,
    angleDelta,
    timeDelta,
    scanTime,
    rangeMin,
    rangeMax,
    ranges,
    intensities,
    timestamp,
  }: LidarFrameMessageProtobuf) {
    this.angleStart = angleStart;
    this.angleEnd = angleEnd;
    this.angleDelta = angleDelta;
    this.timeDelta = timeDelta;
    this.scanTime = scanTime;
    this.rangeMin = rangeMin;
    this.rangeMax = rangeMax;
    this.ranges = ranges;
    this.intensities = intensities;
    this.timestamp = timestamp;
  }
}
