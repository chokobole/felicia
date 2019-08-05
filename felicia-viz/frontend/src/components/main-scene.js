import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { ActionManager } from '@babylonjs/core/Actions/actionManager';
import { ExecuteCodeAction } from '@babylonjs/core/Actions/directActions';
import { Engine } from '@babylonjs/core/Engines/engine';
import { Scene } from '@babylonjs/core/scene';
import { Vector3, Color3 } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import { GridMaterial } from '@babylonjs/materials/grid';
import '@babylonjs/core/Meshes/meshBuilder';

import UI_TYPES from 'store/ui/ui-types';
import OccupancyGridMap from './occupancy-grid-map';

@inject('store')
@observer
export default class MainScene extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    store: PropTypes.object.isRequired,
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

    const camera = new ArcRotateCamera('camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, -30, 30);
    camera.attachControl(this.canvas, true);

    const light = new HemisphericLight('light', new Vector3(0, 0, 1), scene);
    light.intensity = 0.7;

    const material = new GridMaterial('grid', scene);
    material.mainColor = backgroundColor;
    material.opacity = 0.8;

    const plane = Mesh.CreatePlane('plane', 10, scene, true, Mesh.DOUBLESIDE);
    plane.material = material;
    const actionManager = new ActionManager(scene);
    actionManager.registerAction(
      new ExecuteCodeAction(
        {
          trigger: ActionManager.OnKeyDownTrigger,
          parameter: 'c',
        },
        () => {
          const { store } = this.props;
          const { uiState } = store;
          uiState.activeViewState.set(0, UI_TYPES.MainScene.name);
        }
      )
    );
    scene.actionManager = actionManager;

    engine.runRenderLoop(() => {
      const { store } = this.props;
      const viewState = store.uiState.findView(0);
      const { map } = viewState;

      if (map) {
        const { size, resolution, origin, data } = map;
        const { width, height } = size;
        if (!this.map || (this.width !== width || this.height !== height)) {
          this.map = new OccupancyGridMap('occupancy-grid-map', width, height, 1, scene);
        }
        this.map.setOrigin(origin);
        this.map.setResolution(resolution);
        this.map.update(width, height, data);
      }

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
