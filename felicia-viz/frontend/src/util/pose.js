import { createAxis } from 'util/babylon-util';

export default class Pose {
  update(size, pose, scene) {
    if (!this.mesh) {
      this.mesh = createAxis(size, scene);
    }

    const { point, theta } = pose;
    const { x, y } = point;
    this.mesh.position.x = x;
    this.mesh.position.y = y;
    this.mesh.rotation.z = theta;
  }
}
