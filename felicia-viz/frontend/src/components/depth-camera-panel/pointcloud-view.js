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

import { ResizeDetector } from '@felicia-viz/ui';

import { CameraFrame } from 'store/ui/camera-panel-state';
import Worker from 'util/pointcloud-view-webworker.js';

export default class PointcloudView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    frame: PropTypes.instanceOf(CameraFrame),
    filter: PropTypes.string,
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
    filter: '',
  };

  meshInfo = {};

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      const { colors, positions } = event.data;
      const { mesh } = this.meshInfo;
      this.meshInfo.toUpdate -= 1;

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

    const camera = new ArcRotateCamera('camera', 0, 0, 5, new Vector3(0, -20, 10), scene);
    camera.attachControl(this.canvas, true);
    this.camera = camera;

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;
    if (frame !== nextProps.frame) {
      const { width, height } = nextProps.frame;
      if (!frame || width !== this.meshInfo.width || height !== this.meshInfo.height) {
        this._createPointcloud(width, height);
        this._moveCamera(width, height);
      }

      if (this.meshInfo.toUpdate < 5) {
        this._updatePointcloud(nextProps.frame);
        return true;
      }
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

  _onResize = entry => {
    const { width, height } = entry.contentRect;
    this.canvas.width = width;
    this.canvas.height = height;
  };

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  _createPointcloud(width, height) {
    const positions = [];
    const indices = [];
    const colors = [];
    const normals = [];

    for (let i = 0; i < height; i += 1) {
      for (let j = 0; j < width; j += 1) {
        positions.push(width - j - 1);
        positions.push(height - i - 1);
        positions.push(0);

        colors.push(0);
        colors.push(0);
        colors.push(0);
        colors.push(1);
      }
    }

    let idx = 0;
    for (let i = 0; i < height - 1; i += 1) {
      for (let j = 0; j < width; j += 1) {
        if (j !== width - 1) {
          indices.push(idx);
          indices.push(idx + width);
          indices.push(idx + 1);
          indices.push(idx + 1);
          indices.push(idx + width);
          indices.push(idx + width + 1);
        }
        idx += 1;
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
      toUpdate: 0,
    };
  }

  _updatePointcloud(frame) {
    const { width, height, data, pixelFormat, scale } = frame;
    const { mesh } = this.meshInfo;
    const { filter } = this.props;

    const colors = mesh.getVerticesData(VertexBuffer.ColorKind);
    const positions = mesh.getVerticesData(VertexBuffer.PositionKind);
    this.meshInfo.toUpdate += 1;

    this.worker.postMessage({
      source: 'pointcloudView',
      data: {
        colors,
        positions,
        width,
        height,
        data,
        pixelFormat,
        scale,
        filter,
      },
    });
  }

  _moveCamera(width, height) {
    this.camera.setTarget(new Vector3(width / 2, height / 2, 0));
    this.camera.position = new Vector3(
      width / 2,
      height / 2,
      (Math.max(width, height) * Math.SQRT2) / 2
    );
  }

  render() {
    const { width, height } = this.props;
    const margin = '30px';

    const style = {
      width: `calc(${width} - ${margin} * 2)`,
      height: `calc(${height} - ${margin} * 2)`,
      margin,
    };

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas style={{ outline: 'none' }} ref={this._onCanvasLoad} />
      </div>
    );
  }
}
