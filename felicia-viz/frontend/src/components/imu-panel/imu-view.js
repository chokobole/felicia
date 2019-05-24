import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3, Quaternion } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import '@babylonjs/core/Meshes/meshBuilder';

import { Imu } from 'store/ui/imu-panel-state';
import { drawAxis } from 'util/babylon-util';

export default class ImuView extends PureComponent {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    imu: PropTypes.instanceOf(Imu),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    imu: null,
  };

  componentDidMount() {
    const engine = new Engine(this.canvas);

    const backgroundColor = new Color3(51 / 255, 51 / 255, 51 / 255);

    const scene = new Scene(engine);
    scene.clearColor = backgroundColor;

    const camera = new ArcRotateCamera('camera', 0, 0, 5, new Vector3(0, -20, 10), scene);
    camera.setTarget(Vector3.Zero());
    camera.attachControl(this.canvas, true);

    const localOrigin = drawAxis(10, scene);

    engine.runRenderLoop(() => {
      const { imu } = this.props;

      if (imu) {
        const { x, y, z, w } = imu.orientation;
        // TODO(chokobole): If i don't put - to |y| and |z|, yawn and roll is
        // mirrored. It needs to be tested with a different IMU device.
        const orientation = new Quaternion(x, -y, -z, w);
        localOrigin.rotationQuaternion = orientation;
      }

      scene.render();
    });
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  render() {
    const { width, height } = this.props;
    const margin = '30px';

    const style = {
      width: `calc(${width} - ${margin} * 2)`,
      height: `calc(${height} - ${margin} * 2)`,
      margin,
      outline: 'none',
    };

    return <canvas style={style} ref={this._onCanvasLoad} />;
  }
}
