/* eslint import/prefer-default-export: "off" */
import { Quaternion, Vector2, Vector3 } from '@babylonjs/core/Maths/math';

export const SIZEI_MESSAGE = 'felicia.SizeiMessage';
export const SIZEF_MESSAGE = 'felicia.SizefMessage';
export const SIZED_MESSAGE = 'felicia.SizedMessage';
export const QUATERNIONF_MESSAGE = 'felicia.QuaternionfMessage';
export const QUATERNIOND_MESSAGE = 'felicia.QuaterniondMessage';
export const VECTORI_MESSAGE = 'felicia.VectoriMessage';
export const VECTORF_MESSAGE = 'felicia.VectorfMessage';
export const VECTORD_MESSAGE = 'felicia.VectordMessage';
export const VECTOR3I_MESSAGE = 'felicia.Vector3iMessage';
export const VECTOR3F_MESSAGE = 'felicia.Vector3fMessage';
export const VECTOR3D_MESSAGE = 'felicia.Vector3dMessage';
export const POINTI_MESSAGE = 'felicia.PointiMessage';
export const POINTF_MESSAGE = 'felicia.PointfMessage';
export const POINTD_MESSAGE = 'felicia.PointdMessage';
export const POINT3I_MESSAGE = 'felicia.Point3iMessage';
export const POINT3F_MESSAGE = 'felicia.Point3fMessage';
export const POINT3D_MESSAGE = 'felicia.Point3dMessage';
export const POSEF_MESSAGE = 'felicia.PosefMessage';
export const POSED_MESSAGE = 'felicia.PosedMessage';
export const POSE3F_MESSAGE = 'felicia.Pose3fMessage';
export const POSE3D_MESSAGE = 'felicia.Pose3dMessage';
export const POSEF_WITH_TIMESTAMP_MESSAGE = 'felicia.PosefWithTimestampMessage';
export const POSED_WITH_TIMESTAMP_MESSAGE = 'felicia.PosedWithTimestampMessage';
export const POSE3F_WITH_TIMESTAMP_MESSAGE = 'felicia.Pose3fWithTimestampMessage';
export const POSE3D_WITH_TIMESTAMP_MESSAGE = 'felicia.Pose3dWithTimestampMessage';
export const RECTI_MESSAGE = 'felicia.RectiMessage';
export const RECTF_MESSAGE = 'felicia.RectfMessage';
export const RECTD_MESSAGE = 'felicia.RectdMessage';
export const QUADI_MESSAGE = 'felicia.QuadiMessage';
export const QUADF_MESSAGE = 'felicia.QuadfMessage';
export const QUADD_MESSAGE = 'felicia.QuaddMessage';

export interface SizeMessageProtobuf {
  width: number;
  height: number;
}

export class SizeMessage {
  width: number;

  height: number;

  constructor({ width, height }: SizeMessageProtobuf) {
    this.width = width;
    this.height = height;
  }

  toString(): string {
    const { width, height } = this;
    return `(${width}, ${height})`;
  }

  toShortString(): string {
    const { width, height } = this;
    return `(${width.toFixed(2)}, ${height.toFixed(2)})`;
  }
}

export interface QuaternionMessageProtobuf {
  x: number;
  y: number;
  z: number;
  w: number;
}

export class QuaternionMessage {
  x: number;

  y: number;

  z: number;

  w: number;

  constructor({ x, y, z, w }: QuaternionMessageProtobuf) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.w = w;
  }

  toBabylonQuaternion(): Quaternion {
    const { x, y, z, w } = this;
    return new Quaternion(x, y, z, w);
  }

  toString(): string {
    const { x, y, z, w } = this;
    return `(${x}, ${y}, ${z}, ${w})`;
  }

  toShortString(): string {
    const { x, y, z, w } = this;
    return `(${x.toFixed(2)}, ${y.toFixed(2)}, ${z.toFixed(2)}, ${w.toFixed(2)})`;
  }
}

class Vector2Base {
  x = 0;

  y = 0;

  toBabylonVector2(): Vector2 {
    const { x, y } = this;
    return new Vector2(x, y);
  }

  toBabylonVector3(): Vector3 {
    const { x, y } = this;
    return new Vector3(x, y, 0);
  }

  toString(): string {
    const { x, y } = this;
    return `(${x}, ${y})`;
  }

  toShortString(): string {
    const { x, y } = this;
    return `(${x.toFixed(2)}, ${y.toFixed(2)})`;
  }
}

class Vector3Base {
  x = 0;

  y = 0;

  z = 0;

  toBabylonVector2(): Vector2 {
    const { x, y } = this;
    return new Vector2(x, y);
  }

  toBabylonVector3(): Vector3 {
    const { x, y, z } = this;
    return new Vector3(x, y, z);
  }

  toString(): string {
    const { x, y, z } = this;
    return `(${x}, ${y}, ${z})`;
  }

  toShortString(): string {
    const { x, y, z } = this;
    return `(${x.toFixed(2)}, ${y.toFixed(2)}, ${z.toFixed(2)})`;
  }
}

export interface VectorMessageProtobuf {
  x: number;
  y: number;
}

export class VectorMessage extends Vector2Base {
  constructor({ x, y }: VectorMessageProtobuf) {
    super();
    this.x = x;
    this.y = y;
  }
}

export interface Vector3MessageProtobuf {
  x: number;
  y: number;
  z: number;
}

export class Vector3Message extends Vector3Base {
  constructor({ x, y, z }: Vector3MessageProtobuf) {
    super();
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

export interface PointMessageProtobuf {
  x: number;
  y: number;
}

export class PointMessage extends Vector2Base {
  constructor({ x, y }: PointMessageProtobuf) {
    super();
    this.x = x;
    this.y = y;
  }
}

export interface Point3MessageProtobuf {
  x: number;
  y: number;
  z: number;
}

export class Point3Message extends Vector3Base {
  constructor({ x, y, z }: Point3MessageProtobuf) {
    super();
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

export interface PoseMessageProtobuf {
  position: PointMessageProtobuf;
  theta: number;
}

export class PoseMessage {
  position: PointMessage;

  theta: number;

  constructor({ position, theta }: PoseMessageProtobuf) {
    this.position = new PointMessage(position);
    this.theta = theta;
  }
}

export interface Pose3MessageProtobuf {
  position: Point3MessageProtobuf;
  orientation: QuaternionMessageProtobuf;
}

export class Pose3Message {
  position: Point3Message;

  orientation: QuaternionMessage;

  constructor({ position, orientation }: Pose3MessageProtobuf) {
    this.position = new Point3Message(position);
    this.orientation = new QuaternionMessage(orientation);
  }
}

export interface PoseWithTimestampMessageProtobuf extends PoseMessageProtobuf {
  timestamp: number;
}

export class PoseWithTimestampMessage extends PoseMessage {
  timestamp: number;

  constructor(message: PoseWithTimestampMessageProtobuf) {
    super(message);
    const { timestamp } = message;
    this.timestamp = timestamp;
  }
}

export interface Pose3WithTimestampMessageProtobuf extends Pose3MessageProtobuf {
  timestamp: number;
}

export class Pose3WithTimestampMessage extends Pose3Message {
  timestamp: number;

  constructor(message: Pose3WithTimestampMessageProtobuf) {
    super(message);
    const { timestamp } = message;
    this.timestamp = timestamp;
  }
}

export interface RectMessageProtobuf {
  topLeft: PointMessageProtobuf;
  bottomRight: PointMessageProtobuf;
}

export class RectMessage {
  topLeft: PointMessage;

  bottomRight: PointMessage;

  constructor({ topLeft, bottomRight }: RectMessageProtobuf) {
    this.topLeft = new PointMessage(topLeft);
    this.bottomRight = new PointMessage(bottomRight);
  }
}

export interface QuadMessageProtobuf {
  topLeft: PointMessageProtobuf;
  topRight: PointMessageProtobuf;
  bottomLeft: PointMessageProtobuf;
  bottomRight: PointMessageProtobuf;
}

export class QuadMessage {
  topLeft: PointMessage;

  topRight: PointMessage;

  bottomLeft: PointMessage;

  bottomRight: PointMessage;

  constructor({ topLeft, topRight, bottomLeft, bottomRight }: QuadMessageProtobuf) {
    this.topLeft = new PointMessage(topLeft);
    this.topRight = new PointMessage(topRight);
    this.bottomLeft = new PointMessage(bottomLeft);
    this.bottomRight = new PointMessage(bottomRight);
  }
}
