/* eslint import/prefer-default-export: "off" */
import { toVector3, toQuaternion } from 'util/babylon-util';

export class ImuFrameMessage {
  constructor(message) {
    const { orientation, angularVelocity, linearAcceleration, timestamp } = message;
    this.orientation = toQuaternion(orientation);
    this.angularVelocity = toVector3(angularVelocity);
    this.linearAcceleration = toVector3(linearAcceleration);
    this.timestamp = timestamp;
  }
}
