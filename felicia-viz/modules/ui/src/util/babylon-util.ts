/* eslint import/prefer-default-export: "off" */
import { Engine } from '@babylonjs/core/Engines/engine';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import { Color3, Color4, Vector3 } from '@babylonjs/core/Maths/math';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { VertexData } from '@babylonjs/core/Meshes/mesh.vertexData';
import { TransformNode } from '@babylonjs/core/Meshes/transformNode';
import { Scene } from '@babylonjs/core/scene';

export function backgroundColor3(): Color3 {
  return new Color3(51 / 255, 51 / 255, 51 / 255);
}

export function backgroundColor4(): Color4 {
  return new Color4(51 / 255, 51 / 255, 51 / 255, 1);
}

export function createPointcloud(size: number, scene: Scene): Mesh {
  const width = Math.ceil(size / 2);
  const height = 2;

  const positions = new Float32Array(width * height * 3);
  const indices = new Uint32Array((width - 1) * (height - 1) * 6);
  const colors = new Float32Array(width * height * 4);
  const normals = new Float32Array(positions.length);

  let positionsIdx = 0;
  let colorsIdx = 0;
  const centerX = width / 2;
  const centerY = height / 2;
  for (let i = 0; i < height; i += 1) {
    for (let j = 0; j < width; j += 1) {
      positions[positionsIdx] = centerX - j - 1;
      positions[positionsIdx + 1] = centerY - i - 1;
      positions[positionsIdx + 2] = 0;

      colors[colorsIdx] = 1;
      colors[colorsIdx + 1] = 0;
      colors[colorsIdx + 2] = 0;
      colors[colorsIdx + 3] = 1;

      positionsIdx += 3;
      colorsIdx += 4;
    }
  }

  let indicesIdx = 0;
  let verticesIdx = 0;
  for (let i = 0; i < height - 1; i += 1) {
    for (let j = 0; j < width; j += 1) {
      if (j !== width - 1) {
        indices[indicesIdx] = verticesIdx;
        indices[indicesIdx + 1] = verticesIdx + width;
        indices[indicesIdx + 2] = verticesIdx + 1;
        indices[indicesIdx + 3] = verticesIdx + 1;
        indices[indicesIdx + 4] = verticesIdx + width;
        indices[indicesIdx + 5] = verticesIdx + width + 1;
        indicesIdx += 6;
      }
      verticesIdx += 1;
    }
  }

  const material = new StandardMaterial('material', scene);
  material.backFaceCulling = false;
  material.pointsCloud = true;
  material.pointSize = 1;

  const mesh = new Mesh('pointcloud', scene);
  mesh.material = material;
  const vertexData = new VertexData();
  VertexData.ComputeNormals(positions, indices, normals);
  vertexData.positions = positions;
  vertexData.indices = indices;
  vertexData.normals = normals;
  vertexData.colors = colors;
  vertexData.applyToMesh(mesh, true);

  return mesh;
}

export function createAxis(size: number, scene: Scene): TransformNode {
  const axisX = Mesh.CreateLines(
    'axisX',
    [
      Vector3.Zero(),
      new Vector3(size, 0, 0),
      new Vector3(size * 0.95, 0.05 * size, 0),
      new Vector3(size, 0, 0),
      new Vector3(size * 0.95, -0.05 * size, 0),
    ],
    scene
  );
  axisX.color = new Color3(1, 0, 0);
  const axisY = Mesh.CreateLines(
    'axisY',
    [
      Vector3.Zero(),
      new Vector3(0, size, 0),
      new Vector3(-0.05 * size, size * 0.95, 0),
      new Vector3(0, size, 0),
      new Vector3(0.05 * size, size * 0.95, 0),
    ],
    scene
  );
  axisY.color = new Color3(0, 1, 0);
  const axisZ = Mesh.CreateLines(
    'axisZ',
    [
      Vector3.Zero(),
      new Vector3(0, 0, size),
      new Vector3(0, -0.05 * size, size * 0.95),
      new Vector3(0, 0, size),
      new Vector3(0, 0.05 * size, size * 0.95),
    ],
    scene
  );
  axisZ.color = new Color3(0, 0, 1);

  const node = new TransformNode('axis');
  axisX.parent = node;
  axisY.parent = node;
  axisZ.parent = node;

  return node;
}

export function createScene(
  canvas: HTMLCanvasElement
): {
  engine: Engine;
  scene: Scene;
} {
  const engine = new Engine(canvas);

  const scene = new Scene(engine);
  scene.clearColor = backgroundColor4();

  return {
    engine,
    scene,
  };
}
