/* eslint import/prefer-default-export: "off" */
import { Engine } from '@babylonjs/core/Engines/engine';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import { Color3, Vector3 } from '@babylonjs/core/Maths/math';
import { TransformNode } from '@babylonjs/core/Meshes/transformNode';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { VertexData } from '@babylonjs/core/Meshes/mesh.vertexData';
import { Scene } from '@babylonjs/core/scene';

export function backgroundColor() {
  return new Color3(51 / 255, 51 / 255, 51 / 255);
}

export function createPointcloud(size, scene) {
  const width = Math.ceil(size / 2);
  const height = 2;

  const positions = new Float32Array(width * height * 3);
  const indices = new Float32Array((width - 1) * (height - 1) * 6);
  const colors = new Float32Array(width * height * 4);
  const normals = [];

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

export function createAxis(size, scene) {
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

export function createFrustrum(camera, scene) {
  const c = new Color3(0.5, 0.5, 0.5);
  const l = Math.tan(camera.fov / 2) * camera.maxZ;
  const frustrum1 = Mesh.CreateLines(
    'frustrum1',
    [Vector3.Zero(), new Vector3(l, l, camera.maxZ)],
    scene
  );
  frustrum1.color = c;
  const frustrum2 = Mesh.CreateLines(
    'frustrum2',
    [Vector3.Zero(), new Vector3(l, -l, camera.maxZ)],
    scene
  );
  frustrum2.color = c;
  const frustrum3 = Mesh.CreateLines(
    'frustrum3',
    [Vector3.Zero(), new Vector3(-l, l, camera.maxZ)],
    scene
  );
  frustrum3.color = c;
  const frustrum4 = Mesh.CreateLines(
    'frustrum4',
    [Vector3.Zero(), new Vector3(-l, -l, camera.maxZ)],
    scene
  );
  frustrum4.color = c;

  const node = new TransformNode('frustrum');
  frustrum1.parent = node;
  frustrum2.parent = node;
  frustrum3.parent = node;
  frustrum4.parent = node;

  return node;
}

export function createScene(canvas) {
  const engine = new Engine(canvas);

  const scene = new Scene(engine);
  scene.clearColor = backgroundColor();

  return {
    engine,
    scene,
  };
}
