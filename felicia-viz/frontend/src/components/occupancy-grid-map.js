/* eslint no-bitwise: ["off"] */
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import { DynamicTexture } from '@babylonjs/core/Materials/Textures/dynamicTexture';

export default class OccupancyGridMap {
  constructor(name, width, height, subdivision, scene) {
    const ground = Mesh.CreateGround(name, width, height, subdivision, scene, true);
    const dynamicTexture = new DynamicTexture(`${name}-dynamic-texture`, { width, height }, scene);

    const materialGround = new StandardMaterial(`${name}-mat`, scene);
    materialGround.diffuseTexture = dynamicTexture;
    materialGround.diffuseTexture.hasAlpha = true;
    ground.material = materialGround;

    this.width = width;
    this.height = height;
    this.ground = ground;
    this.dynamicTexture = dynamicTexture;
  }

  toMainSceneCoordinate() {
    this.ground.addRotation(Math.PI / 2, 0, 0).addRotation(0, Math.PI, 0);
  }

  setOrigin(origin) {
    if (this.origin && this.origin.x === origin.x && this.origin.y === origin.y) return;
    const x = -this.width / 2;
    const y = -this.height / 2;
    const deltaX = x - origin.x;
    const deltaY = y - origin.y;
    this.ground.position.x += deltaX;
    this.ground.position.z += deltaY;
    this.origin = origin;
  }

  setResolution(resolution) {
    if (this.resolution && this.resolution === resolution) return;
    this.ground.scaling.x *= resolution;
    this.ground.scaling.y *= resolution;
    this.ground.scaling.z *= resolution;
    this.resolution = resolution;
  }

  update(width, height, data) {
    const ctx = this.dynamicTexture.getContext();
    const imageData = ctx.getImageData(0, 0, width, height);
    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        const idx = i * width + j;
        const pixelsIdx = idx << 2;
        const pixels = imageData.data;
        const v = data[idx];
        if (v >= 0 && v <= 100) {
          const c = Math.round(255 * (1 - v / 100));
          pixels[pixelsIdx] = c;
          pixels[pixelsIdx + 1] = c;
          pixels[pixelsIdx + 2] = c;
          pixels[pixelsIdx + 3] = 255;
        } else {
          pixels[pixelsIdx + 3] = 0;
        }
      }
    }
    ctx.putImageData(imageData, 0, 0);
    this.dynamicTexture.update();
  }
}
