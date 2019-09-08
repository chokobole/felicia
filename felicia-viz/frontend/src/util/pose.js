import { createAxis } from 'util/babylon-util';

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
