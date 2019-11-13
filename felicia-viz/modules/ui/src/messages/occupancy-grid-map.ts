// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import { DynamicTexture } from '@babylonjs/core/Materials/Textures/dynamicTexture';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { MeshBuilder } from '@babylonjs/core/Meshes/meshBuilder';
import { Scene } from '@babylonjs/core/scene';
import { PointMessage } from '@felicia-viz/proto/messages/geometry';
import { OccupancyGridMapMessage } from '@felicia-viz/proto/messages/map-message';
import { OutputEvent } from '../webworkers/occupancy-grid-map-webworker';

export default class OccupancyGridMap {
  constructor(worker: Worker, width: number, height: number, scene: Scene) {
    // FIXME: If it directly loads worker here, then it can't build.
    this.worker = worker;

    this.worker.onmessage = (event: OutputEvent): void => {
      if (!this.dynamicTexture) return;
      this.dynamicTexture.getContext().putImageData(event.data, 0, 0);
      this.dynamicTexture.update();
    };

    if (width && height && scene) {
      this._createMesh('occupancy-grid-map', width, height, scene);
    }
  }

  private worker: Worker;

  private width = 0;

  private height = 0;

  private mesh?: Mesh;

  private dynamicTexture?: DynamicTexture;

  private origin?: PointMessage;

  private resolution = 0;

  update(occupancyGridMap: OccupancyGridMapMessage | null, scene: Scene): void {
    if (!occupancyGridMap) return;

    const { size, resolution, origin } = occupancyGridMap;
    const { width, height } = size;

    if (!this.mesh || this.width !== width || this.height !== height) {
      this._createMesh('occupancy-grid-map', width, height, scene);
    }
    this._setOrigin(origin);
    this._setResolution(resolution);

    if (this.dynamicTexture)
      this.worker.postMessage({
        imageData: this.dynamicTexture.getContext().getImageData(0, 0, width, height),
        occupancyGridMap,
      });
  }

  private _createMesh(name: string, width: number, height: number, scene: Scene): void {
    const plane = MeshBuilder.CreatePlane(
      `${name}-plane`,
      { width, height, updatable: true, sideOrientation: Mesh.BACKSIDE },
      scene
    );
    const dynamicTexture = new DynamicTexture(
      `${name}-dynamic-texture`,
      { width, height },
      scene,
      false
    );

    const material = new StandardMaterial(`${name}-mat`, scene);
    material.diffuseTexture = dynamicTexture;
    material.diffuseTexture.hasAlpha = true;
    plane.material = material;

    this.width = width;
    this.height = height;
    this.mesh = plane;
    this.dynamicTexture = dynamicTexture;
  }

  private _setOrigin(origin: PointMessage): void {
    if (!this.mesh) return;
    if (this.origin && this.origin.x === origin.x && this.origin.y === origin.y) return;
    const x = -this.width / 2;
    const y = -this.height / 2;
    const deltaX = x - origin.x;
    const deltaY = y - origin.y;
    this.mesh.position.x += deltaX;
    this.mesh.position.y += deltaY;
    this.origin = origin;
  }

  private _setResolution(resolution: number): void {
    if (!this.mesh) return;
    if (this.resolution && this.resolution === resolution) return;
    this.mesh.scaling.x *= resolution;
    this.mesh.scaling.y *= resolution;
    this.mesh.scaling.z *= resolution;
    this.resolution = resolution;
  }

  terminateWorker(): void {
    this.worker.terminate();
  }
}
