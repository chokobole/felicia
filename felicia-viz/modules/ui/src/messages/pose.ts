import { TransformNode } from '@babylonjs/core/Meshes/transformNode';
import { Scene } from '@babylonjs/core/scene';
import {
  Pose3WithTimestampMessage,
  PoseWithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
import { createAxis } from '../util/babylon-util';

export default class Pose {
  constructor(size: number, scene: Scene) {
    this.mesh = createAxis(size, scene);
  }

  mesh: TransformNode;

  update(pose: PoseWithTimestampMessage | Pose3WithTimestampMessage | null): void {
    if (!pose) return;

    if ((pose as Pose3WithTimestampMessage).orientation) {
      const { position, orientation } = pose as Pose3WithTimestampMessage;
      this.mesh.position = position.toBabylonVector3();
      this.mesh.rotationQuaternion = orientation.toBabylonQuaternion();
    } else {
      const { position, theta } = pose as PoseWithTimestampMessage;
      this.mesh.position = position.toBabylonVector3();
      this.mesh.rotation.z = theta;
    }
  }
}
