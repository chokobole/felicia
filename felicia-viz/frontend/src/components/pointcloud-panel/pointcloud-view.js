import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3 } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { VertexBuffer } from '@babylonjs/core/Meshes/buffer';
import '@babylonjs/core/Meshes/meshBuilder';

import { babylonCanvasStyle } from 'custom-styles';
import { PointcloudFrame } from 'store/ui/pointcloud-panel-state';
import { drawAxis, drawFrustrum, makePointcloud } from 'util/babylon-util';
import DataMessageReader from 'util/data-message-reader';
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

  pointcloud = {};

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      const { colors, positions } = event.data;
      const { mesh } = this.pointcloud;

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
      const size = new DataMessageReader(points).length();
      if (!frame || this.pointcloud.size < size) {
        if (this.pointcloud.mesh) {
          this.pointcloud.mesh.dispose();
        }
        this.pointcloud.mesh = makePointcloud(size, this.scene);
        this.pointcloud.size = size;
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

  _updatePointcloud(frame) {
    const { mesh } = this.pointcloud;

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
