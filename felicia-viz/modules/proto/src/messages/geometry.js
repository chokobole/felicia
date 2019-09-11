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

export class SizeMessage {
  constructor(message) {
    const { width, height } = message;
    this.width = width;
    this.height = height;
  }
}

export class QuaternionMessage {
  constructor(message) {
    const { w, x, y, z } = message;
    this.w = w;
    this.x = x;
    this.y = y;
    this.z = z;
  }

  toBabylonQuaternion() {
    const { x, y, z, w } = this;
    return new Quaternion(x, y, z, w);
  }
}

class BabylonVectorConvertable2 {
  toBabylonVector2() {
    const { x, y } = this;
    return new Vector2(x, y);
  }

  toBabylonVector3() {
    const { x, y } = this;
    return new Vector3(x, y, 0);
  }
}

class BabylonVectorConvertable3 {
  toBabylonVector2() {
    const { x, y } = this;
    return new Vector2(x, y);
  }

  toBabylonVector3() {
    const { x, y, z } = this;
    return new Vector3(x, y, z);
  }
}

export class VectorMessage extends BabylonVectorConvertable2 {
  constructor(message) {
    super();
    const { x, y } = message;
    this.x = x;
    this.y = y;
  }
}

export class Vector3Message extends BabylonVectorConvertable3 {
  constructor(message) {
    super();
    const { x, y, z } = message;
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

export class PointMessage extends BabylonVectorConvertable2 {
  constructor(message) {
    super();
    const { x, y } = message;
    this.x = x;
    this.y = y;
  }
}

export class Point3Message extends BabylonVectorConvertable3 {
  constructor(message) {
    super();
    const { x, y, z } = message;
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

export class PoseMessage {
  constructor(message) {
    const { position, theta } = message;
    this.position = new PointMessage(position);
    this.theta = theta;
  }
}

export class Pose3Message {
  constructor(message) {
    const { position, orientation } = message;
    this.position = new Point3Message(position);
    this.orientation = new QuaternionMessage(orientation);
  }
}

export class PoseWithTimestampMessage extends PoseMessage {
  constructor(message) {
    super(message);
    const { timestamp } = message;
    this.timestamp = timestamp;
  }
}

export class Pose3WithTimestampMessage extends Pose3Message {
  constructor(message) {
    super(message);
    const { timestamp } = message;
    this.timestamp = timestamp;
  }
}

export class RectMessage {
  constructor(message) {
    const { topLeft, bottomRight } = message;
    this.topLeft = new PointMessage(topLeft);
    this.bottomRight = new PointMessage(bottomRight);
  }
}

export class QuadMessage {
  constructor(message) {
    const { topLeft, topRight, bottomLeft, bottomRight } = message;
    this.topLeft = new PointMessage(topLeft);
    this.topRight = new PointMessage(topRight);
    this.bottomLeft = new PointMessage(bottomLeft);
    this.bottomRight = new PointMessage(bottomRight);
  }
}
