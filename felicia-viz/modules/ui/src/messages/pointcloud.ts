/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["colors", "positions"] }] */
import { Mesh } from '@babylonjs/core';
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';
import { Scene } from '@babylonjs/core/scene';
import { Points } from '@felicia-viz/proto/messages/data-message';
import { PointcloudMessage } from '@felicia-viz/proto/messages/map-message';
import { createPointcloud } from '../util/babylon-util';
import { MeshInfo, OutputEvent } from '../webworkers/pointcloud-webworker';

export default class Pointcloud {
  constructor(worker: Worker) {
    // FIXME: If it directly loads worker here, then it can't build.
    this.worker = worker;
    this.meshMap = new Map<number, Mesh>();
    this.id = 0;
    this.size = 0;

    this.worker.onmessage = (event: OutputEvent): void => {
      const meshInfos = event.data;
      for (let i = 0; i < meshInfos.length; i += 1) {
        const { id, colors, positions } = meshInfos[i];
        const mesh = this.meshMap.get(id);
        if (mesh) {
          mesh.updateVerticesData(VertexBuffer.ColorKind, colors);
          mesh.updateVerticesData(VertexBuffer.PositionKind, positions, true);
        }
      }
    };
  }

  private worker: Worker;

  private meshMap: Map<number, Mesh>;

  private id: number;

  private size: number;

  private _createPointcloud(size: number, scene: Scene): void {
    const unitSize = 10000;
    while (this.size < size) {
      this.meshMap.set(this.id, createPointcloud(`pointcloud-${this.id}`, unitSize, scene));
      this.size += unitSize;
      this.id += 1;
    }
  }

  update(frame: PointcloudMessage | null, scene: Scene): void {
    if (!frame) return;

    const { points } = frame;
    const size = new Points(points).length();
    this._createPointcloud(size, scene);

    const meshInfos: Array<MeshInfo> = [];
    this.meshMap.forEach(
      (value: Mesh, key: number): void => {
        const colors = value.getVerticesData(VertexBuffer.ColorKind);
        const positions = value.getVerticesData(VertexBuffer.PositionKind);
        if (!(colors && positions)) return;
        meshInfos.push({
          id: key,
          colors,
          positions,
        });
      }
    );

    this.worker.postMessage({
      meshInfos,
      frame,
    });
  }

  terminateWorker(): void {
    this.worker.terminate();
  }
}
