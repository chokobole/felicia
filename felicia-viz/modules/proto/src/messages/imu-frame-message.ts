import {
  QuaternionMessage,
  QuaternionMessageProtobuf,
  Vector3Message,
  Vector3MessageProtobuf,
} from './geometry';

export const IMU_FRAME_MESSAGE = 'felicia.drivers.ImuFrameMessage';

export interface ImuFrameMessageProtobuf {
  orientation: QuaternionMessageProtobuf;
  angularVelocity: Vector3MessageProtobuf;
  linearAcceleration: Vector3MessageProtobuf;
  timestamp: number;
}

export default class ImuFrameMessage {
  orientation: QuaternionMessage;

  angularVelocity: Vector3Message;

  linearAcceleration: Vector3Message;

  timestamp: number;

  constructor({
    orientation,
    angularVelocity,
    linearAcceleration,
    timestamp,
  }: ImuFrameMessageProtobuf) {
    this.orientation = new QuaternionMessage(orientation);
    this.angularVelocity = new Vector3Message(angularVelocity);
    this.linearAcceleration = new Vector3Message(linearAcceleration);
    this.timestamp = timestamp;
  }
}
