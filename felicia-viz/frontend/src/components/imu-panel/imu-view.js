import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { Vector3, Quaternion } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import '@babylonjs/core/Meshes/meshBuilder';

import { babylonCanvasStyle } from 'custom-styles';
import { ImuFrameMessage } from 'store/ui/imu-panel-state';
import { createAxis, createScene } from 'util/babylon-util';

export default class ImuView extends Component {
  static propTypes = {
    width: PropTypes.string, // eslint-disable-line
    height: PropTypes.string, // eslint-disable-line
    frame: PropTypes.instanceOf(ImuFrameMessage),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
  };

  componentDidMount() {
    const { engine, scene } = createScene(this.canvas);

    const camera = new ArcRotateCamera('camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, 0, -10);
    camera.attachControl(this.canvas, true);

    this.localOrigin = createAxis(10, scene);

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;
    if (frame !== nextProps.frame) {
      const { x, y, z, w } = nextProps.frame.orientation;
      this.localOrigin.rotationQuaternion = new Quaternion(x, y, z, w);
      return true;
    }

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  render() {
    return <canvas style={babylonCanvasStyle(this.props)} ref={this._onCanvasLoad} />;
  }
}
