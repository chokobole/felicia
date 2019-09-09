/* eslint import/prefer-default-export: "off" */
export class LidarFrameMessage {
  constructor(message) {
    const {
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
    } = message;
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
