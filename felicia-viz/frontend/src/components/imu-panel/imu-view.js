import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3, Quaternion } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { DynamicTexture } from '@babylonjs/core/Materials/Textures/dynamicTexture';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { StandardMaterial } from '@babylonjs/core/Materials/standardMaterial';
import '@babylonjs/core/Meshes/meshBuilder';

import { Imu } from 'store/ui/imu-panel-state';

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

  static makeTextPlane(text, color, size, scene) {
    const dynamicTexture = new DynamicTexture('DynamicTexture', 50, scene, true);
    dynamicTexture.hasAlpha = true;
    dynamicTexture.drawText(text, 5, 40, 'bold 36px Arial', color, 'transparent', true);
    const plane = new Mesh.CreatePlane('TextPlane', size, scene, true);
    plane.material = new StandardMaterial('TextPlaneMaterial', scene);
    plane.material.backFaceCulling = false;
    plane.material.specularColor = new Color3(0, 0, 0);
    plane.material.diffuseTexture = dynamicTexture;
    return plane;
  }

  static drawAxis(size, scene) {
    const axisX = Mesh.CreateLines(
      'axisX',
      [
        new Vector3.Zero(),
        new Vector3(size, 0, 0),
        new Vector3(size * 0.95, 0.05 * size, 0),
        new Vector3(size, 0, 0),
        new Vector3(size * 0.95, -0.05 * size, 0),
      ],
      scene
    );
    axisX.color = new Color3(1, 0, 0);
    const xChar = ImuView.makeTextPlane('X', 'red', size / 10, scene);
    xChar.position = new Vector3(0.9 * size, -0.05 * size, 0);
    const axisY = Mesh.CreateLines(
      'axisY',
      [
        new Vector3.Zero(),
        new Vector3(0, size, 0),
        new Vector3(-0.05 * size, size * 0.95, 0),
        new Vector3(0, size, 0),
        new Vector3(0.05 * size, size * 0.95, 0),
      ],
      scene
    );
    axisY.color = new Color3(0, 1, 0);
    const yChar = ImuView.makeTextPlane('Y', 'green', size / 10, scene);
    yChar.position = new Vector3(0, 0.9 * size, -0.05 * size);
    const axisZ = Mesh.CreateLines(
      'axisZ',
      [
        new Vector3.Zero(),
        new Vector3(0, 0, size),
        new Vector3(0, -0.05 * size, size * 0.95),
        new Vector3(0, 0, size),
        new Vector3(0, 0.05 * size, size * 0.95),
      ],
      scene
    );
    axisZ.color = new Color3(0, 0, 1);
    const zChar = ImuView.makeTextPlane('Z', 'blue', size / 10, scene);
    zChar.position = new Vector3(0, 0.05 * size, 0.9 * size);

    const localOrigin = Mesh.CreateBox('local_origin', 1, scene);
    localOrigin.isVisible = false;
    axisX.parent = localOrigin;
    axisY.parent = localOrigin;
    axisZ.parent = localOrigin;
    xChar.parent = localOrigin;
    yChar.parent = localOrigin;
    zChar.parent = localOrigin;

    return localOrigin;
  }

  componentDidMount() {
    const engine = new Engine(this.canvas);

    const backgroundColor = new Color3(51 / 255, 51 / 255, 51 / 255);

    const scene = new Scene(engine);
    scene.clearColor = backgroundColor;

    const camera = new ArcRotateCamera('camera', 0, 0, 5, new Vector3(0, -20, 10), scene);
    camera.setTarget(Vector3.Zero());
    camera.attachControl(this.canvas, true);

    const localOrigin = ImuView.drawAxis(10, scene);

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
