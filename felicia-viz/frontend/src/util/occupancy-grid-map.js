import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import { DynamicTexture } from '@babylonjs/core/Materials/Textures/dynamicTexture';
import { MeshBuilder } from '@babylonjs/core/Meshes/meshBuilder';

export default class OccupancyGridMap {
  constructor(worker) {
    this.worker = worker;

    this.worker.onmessage = event => {
      this.dynamicTexture.getContext().putImageData(event.data, 0, 0);
      this.dynamicTexture.update();
    };
  }

  update(occupancyGridMap, scene) {
    const { size, resolution, origin } = occupancyGridMap;
    const { width, height } = size;

    if (!this.mesh || this.width !== width || this.height !== height) {
      this._createMesh('occupancy-grid-map', width, height, scene);
    }
    this._setOrigin(origin);
    this._setResolution(resolution);

    this.worker.postMessage({
      imageData: this.dynamicTexture.getContext().getImageData(0, 0, width, height),
      occupancyGridMap,
    });
  }

  _createMesh(name, width, height, scene) {
    const plane = MeshBuilder.CreatePlane(
      `${name}-plane`,
      { width, height, updatable: true, sideOrientation: Mesh.BACKSIDE },
      scene
    );
    const dynamicTexture = new DynamicTexture(`${name}-dynamic-texture`, { width, height }, scene);

    const material = new StandardMaterial(`${name}-mat`, scene);
    material.diffuseTexture = dynamicTexture;
    material.diffuseTexture.hasAlpha = true;
    plane.material = material;

    this.width = width;
    this.height = height;
    this.mesh = plane;
    this.dynamicTexture = dynamicTexture;
  }

  _setOrigin(origin) {
    if (this.origin && this.origin.x === origin.x && this.origin.y === origin.y) return;
    const x = -this.width / 2;
    const y = -this.height / 2;
    const deltaX = x - origin.x;
    const deltaY = y - origin.y;
    this.mesh.position.x += deltaX;
    this.mesh.position.y += deltaY;
    this.origin = origin;
  }

  _setResolution(resolution) {
    if (this.resolution && this.resolution === resolution) return;
    this.mesh.scaling.x *= resolution;
    this.mesh.scaling.y *= resolution;
    this.mesh.scaling.z *= resolution;
    this.resolution = resolution;
  }

  terminateWorker() {
    this.worker.terminate();
  }
}
