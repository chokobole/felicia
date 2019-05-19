import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3 } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { GridMaterial } from '@babylonjs/materials/grid';
import '@babylonjs/core/Meshes/meshBuilder';

@inject('store')
@observer
export default class MainScene extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
  };

  componentDidMount() {
    const engine = new Engine(this.canvas);

    const backgroundColor = new Color3(51 / 255, 51 / 255, 51 / 255);

    const scene = new Scene(engine);
    scene.clearColor = backgroundColor;

    const camera = new ArcRotateCamera('camera', 0, 0, 5, new Vector3(0, -20, 10), scene);
    camera.setTarget(Vector3.Zero());
    camera.attachControl(this.canvas, true);

    const light = new HemisphericLight('light', new Vector3(0, 0, 1), scene);
    light.intensity = 0.7;

    const material = new GridMaterial('grid', scene);
    material.mainColor = backgroundColor;
    material.opacity = 0.8;

    const plane = Mesh.CreatePlane('plane', 10, scene, true, Mesh.DOUBLESIDE);
    plane.material = material;

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  render() {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return <canvas style={style} ref={this._onCanvasLoad} />;
  }
}
