/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["colors", "positions"] }] */
import { Mesh } from '@babylonjs/core';
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';
import { Scene } from '@babylonjs/core/scene';
import { Points } from '@felicia-viz/proto/messages/data-message';
import { PointcloudMessage } from '@felicia-viz/proto/messages/map-message';
import { createPointcloud } from '../util/babylon-util';
import { OutputEvent } from '../webworkers/pointcloud-webworker';

export default class Pointcloud {
  constructor(worker: Worker) {
    // FIXME: If i directly load worker here, then it can't build.
    this.worker = worker;

    this.worker.onmessage = (event: OutputEvent): void => {
      const { colors, positions } = event.data;

      if (this.mesh) {
        this.mesh.updateVerticesData(VertexBuffer.ColorKind, colors);
        this.mesh.updateVerticesData(VertexBuffer.PositionKind, positions);
      }
    };
  }

  // TODO: Need to implement better allocation strategy.
  private _allocate = (size: number): number => {
    return size > 100000 ? size : 100000;
  };

  private worker: Worker;

  private size?: number;

  private mesh?: Mesh;

  update(frame: PointcloudMessage | null, scene: Scene): void {
    if (!frame) return;

    const { points } = frame;
    const size = new Points(points).length();
    if (!this.mesh || (this.size && this.size < size)) {
      if (this.mesh) {
        this.mesh.dispose();
      }
      const allocationSize = this._allocate(size);
      this.mesh = createPointcloud(allocationSize, scene);
      this.size = allocationSize;
    }

    const colors = this.mesh.getVerticesData(VertexBuffer.ColorKind);
    const positions = this.mesh.getVerticesData(VertexBuffer.PositionKind);
    if (!(colors && positions)) return;

    this.worker.postMessage({
      colors,
      positions,
      frame,
    });
  }

  terminateWorker(): void {
    this.worker.terminate();
  }
}
