import { QuaternionMessage, Vector3Message } from './geometry';

export const IMU_FRAME_MESSAGE = 'felicia.drivers.ImuFrameMessage';

export default class ImuFrameMessage {
  constructor(message) {
    const { orientation, angularVelocity, linearAcceleration, timestamp } = message;
    this.orientation = new QuaternionMessage(orientation);
    this.angularVelocity = new Vector3Message(angularVelocity);
    this.linearAcceleration = new Vector3Message(linearAcceleration);
    this.timestamp = timestamp;
  }
}
