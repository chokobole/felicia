import { createAxis, toVector3, toQuaternion } from 'util/babylon-util';

export default class Pose {
  constructor(size, scene) {
    this.mesh = createAxis(size, scene);
  }

  update(pose) {
    if (pose.is3D) {
      const { position, orientation } = pose;
      this.mesh.position = position;
      this.mesh.rotationQuaternion = orientation;
    } else {
      const { position, theta } = pose;
      this.mesh.position = position;
      this.mesh.rotation.z = theta;
    }
    this.lastTimestamp = this.timestamp;
  }
}

export class PoseWithTimestampMessage {
  constructor(message) {
    const { position, theta, timestamp } = message;
    this.position = toVector3(position);
    this.theta = theta;
    this.timestamp = timestamp;
    this.is3D = false;
  }
}

export class Pose3WithTimestampMessage {
  constructor(message) {
    const { position, orientation, timestamp } = message;
    this.position = toVector3(position);
    this.orientation = toQuaternion(orientation);
    this.timestamp = timestamp;
    this.is3D = true;
  }
}
