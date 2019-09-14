import { createAxis } from '../util/babylon-util';

export default class Pose {
  constructor(size, scene) {
    this.mesh = createAxis(size, scene);
  }

  update(pose) {
    if (pose.orientation) {
      const { position, orientation } = pose;
      this.mesh.position = position.toBabylonVector3();
      this.mesh.rotationQuaternion = orientation.toBabylonQuaternion();
    } else {
      const { position, theta } = pose;
      this.mesh.position = position.toBabylonVector3();
      this.mesh.rotation.z = theta;
    }
    this.lastTimestamp = this.timestamp;
  }
}
