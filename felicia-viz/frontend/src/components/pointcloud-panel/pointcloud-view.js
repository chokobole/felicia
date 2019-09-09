import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { Vector3 } from '@babylonjs/core/Maths/math';
import '@babylonjs/core/Meshes/meshBuilder';

import { babylonCanvasStyle } from 'custom-styles';
import Pointcloud, { PointcloudMessage } from 'messages/pointcloud';
import { createAxis, createFrustrum, createScene } from 'util/babylon-util';
import Worker from 'webworkers/pointcloud-webworker';

export default class PointcloudView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    frame: PropTypes.instanceOf(PointcloudMessage),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
  };

  componentDidMount() {
    this.pointcloud = new Pointcloud(new Worker());

    const { engine, scene } = createScene(this.canvas);
    this.scene = scene;

    const light = new HemisphericLight('hemiLight', new Vector3(0, 1, 0), scene);
    light.intensity = 1;

    const camera = new ArcRotateCamera('camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.setPosition(new Vector3(0, 0, -1));
    camera.lowerRadiusLimit = 1;
    camera.wheelPrecision = 10;
    camera.attachControl(this.canvas, true);

    const axis = createAxis(0.1, scene);
    axis.position = Vector3.Zero();
    createFrustrum(camera, scene);

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;
    if (frame !== nextProps.frame) {
      this.pointcloud.update(nextProps.frame, this.scene);
      return true;
    }

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    this.pointcloud.terminateWorker();
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  render() {
    return <canvas style={babylonCanvasStyle(this.props)} ref={this._onCanvasLoad} />;
  }
}
