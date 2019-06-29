import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3 } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { VertexData } from '@babylonjs/core/Meshes/mesh.vertexData';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import '@babylonjs/core/Meshes/meshBuilder';

import { babylonCanvasStyle } from 'custom-styles';
import { PointcloudFrame } from 'store/ui/pointcloud-panel-state';
import { drawAxis, drawFrustrum } from 'util/babylon-util';
import Worker from 'util/pointcloud-view-webworker.js';

export default class PointcloudView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    frame: PropTypes.instanceOf(PointcloudFrame),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
  };

  meshInfo = {};

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      const { colors, positions } = event.data;
      const { mesh } = this.meshInfo;

      mesh.updateVerticesData(VertexBuffer.ColorKind, colors);
      mesh.updateVerticesData(VertexBuffer.PositionKind, positions);
    };

    const engine = new Engine(this.canvas);

    const backgroundColor = new Color3(51 / 255, 51 / 255, 51 / 255);

    const scene = new Scene(engine);
    scene.clearColor = backgroundColor;

    this.scene = scene;

    const light = new HemisphericLight('hemiLight', new Vector3(0, 1, 0), scene);
    light.intensity = 1;

    const camera = new ArcRotateCamera('camera', 0, 0, 0, new Vector3(0, 0, -2), scene);
    camera.setTarget(new Vector3.Zero());
    camera.lowerRadiusLimit = 1;
    camera.wheelPrecision = 10;
    camera.attachControl(this.canvas, true);
    this.camera = camera;

    const axis = drawAxis(0.1, scene);
    axis.position = new Vector3.Zero();
    drawFrustrum(camera, scene);

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;
    if (frame !== nextProps.frame) {
      const { points } = nextProps.frame;
      const size = points.byteLength / 4;
      const meshSize = this.meshInfo.width * this.meshInfo.height;
      if (!frame || meshSize < size) {
        if (this.meshInfo.mesh) {
          this.meshInfo.mesh.dispose();
        }
        this._createPointcloud(Math.ceil(size / 2), 2);
      }

      this._updatePointcloud(nextProps.frame);
      return true;
    }

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    this.worker.terminate();
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  _createPointcloud(width, height) {
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

    const material = new StandardMaterial('material', this.scene);
    material.backFaceCulling = false;
    material.pointsCloud = true;
    material.pointSize = 1;

    const mesh = new Mesh('pointcloud', this.scene);
    mesh.material = material;
    const vertexData = new VertexData();
    VertexData.ComputeNormals(positions, indices, normals);
    vertexData.positions = positions;
    vertexData.indices = indices;
    vertexData.normals = normals;
    vertexData.colors = colors;
    vertexData.applyToMesh(mesh, true);

    this.meshInfo = {
      mesh,
      width,
      height,
    };
  }

  _updatePointcloud(frame) {
    const { mesh } = this.meshInfo;

    const colors = mesh.getVerticesData(VertexBuffer.ColorKind);
    const positions = mesh.getVerticesData(VertexBuffer.PositionKind);

    this.worker.postMessage({
      colors,
      positions,
      frame,
    });
  }

  render() {
    return <canvas style={babylonCanvasStyle(this.props)} ref={this._onCanvasLoad} />;
  }
}
