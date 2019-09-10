/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["colors", "positions"] }] */
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';

import { DataMessage, Points } from 'messages/data';
import { createPointcloud } from 'util/babylon-util';

export default class Pointcloud {
  constructor(worker, size, scene) {
    // FIXME: If i directly load worker here, then it can't build.
    this.worker = worker;

    this.worker.onmessage = event => {
      const { colors, positions } = event.data;

      this.mesh.updateVerticesData(VertexBuffer.ColorKind, colors);
      this.mesh.updateVerticesData(VertexBuffer.PositionKind, positions);
    };

    if (size && scene) {
      this.mesh = createPointcloud(size, scene);
      this.size = size;
    }
  }

  // TODO: Need to implement better allocation strategy.
  _allocate = size => {
    return size > 100000 ? size : 100000;
  };

  update(frame, scene) {
    const { points } = frame;
    const size = new Points(points).length();
    if (!this.mesh || this.size < size) {
      if (this.mesh) {
        this.mesh.dispose();
      }
      const allocationSize = this._allocate(size);
      this.mesh = createPointcloud(allocationSize, scene);
      this.size = allocationSize;
    }

    const colors = this.mesh.getVerticesData(VertexBuffer.ColorKind);
    const positions = this.mesh.getVerticesData(VertexBuffer.PositionKind);

    this.worker.postMessage({
      colors,
      positions,
      frame,
    });
  }

  terminateWorker() {
    this.worker.terminate();
  }
}

export class PointcloudMessage {
  constructor(message) {
    const { points, colors, timestamp } = message;
    this.points = new DataMessage(points);
    this.colors = new DataMessage(colors);
    this.timestamp = timestamp;
  }
}
