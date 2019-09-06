/* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["colors", "positions"] }] */
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';

import { createPointcloud } from 'util/babylon-util';
import DataMessageReader, { PointReader, ColorReader } from 'util/data-message-reader';

export default class Pointcloud {
  constructor(worker) {
    // FIXME: If i directly load worker here, then it can't build.
    this.worker = worker;

    this.worker.onmessage = event => {
      const { colors, positions } = event.data;

      this.mesh.updateVerticesData(VertexBuffer.ColorKind, colors);
      this.mesh.updateVerticesData(VertexBuffer.PositionKind, positions);
    };
  }

  update(frame, scene) {
    const { points } = frame;
    const size = new DataMessageReader(points).length();
    if (!this.mesh || this.size < size) {
      if (this.mesh) {
        this.mesh.dispose();
      }
      this.mesh = createPointcloud(size, scene);
      this.size = size;
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

export function fillPointcloud(colors, positions, frame) {
  const pointsReader = new PointReader(frame.points);
  const colorsReader = new ColorReader(frame.colors);

  const size = positions.length / 3;
  for (let i = 0; i < size; i += 1) {
    const colorsIdx = i * 4;
    const positionsIdx = i * 3;
    if (pointsReader.hasData(i)) {
      [
        positions[positionsIdx],
        positions[positionsIdx + 1],
        positions[positionsIdx + 2],
      ] = pointsReader.nextPoint3(i);
      if (colorsReader.hasData(i)) {
        [
          colors[colorsIdx],
          colors[colorsIdx + 1],
          colors[colorsIdx + 2],
          colors[colorsIdx + 3],
        ] = colorsReader.nextColor4f(i);
      }
    } else {
      positions[positionsIdx + 2] = -1000000;
    }
  }
}
