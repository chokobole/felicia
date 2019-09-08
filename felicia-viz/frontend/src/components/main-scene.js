import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { ActionManager } from '@babylonjs/core/Actions/actionManager';
import { ExecuteCodeAction } from '@babylonjs/core/Actions/directActions';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { GridMaterial } from '@babylonjs/materials/grid';
import { Vector3 } from '@babylonjs/core/Maths/math';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import '@babylonjs/core/Meshes/meshBuilder';

import UI_TYPES from 'store/ui/ui-types';
import {
  OccupancyGridMapMessage,
  PosefWithTimestampMessage,
  Pose3fWithTimestampMessage,
} from 'store/ui/main-scene-state';
import { PointcloudFrameMessage } from 'store/ui/pointcloud-panel-state';
import { backgroundColor, createScene } from 'util/babylon-util';
import OccupancyGridMap from 'util/occupancy-grid-map';
import OccupancyGridMapWorker from 'util/occupancy-grid-map-webworker.js';
import Pointcloud from 'util/pointcloud';
import PointcloudFrameWorker from 'util/pointcloud-frame-webworker.js';
import Pose from 'util/pose';

@inject('store')
@observer
export default class MainScene extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    store: PropTypes.object.isRequired,
    occupancyGridMap: PropTypes.instanceOf(OccupancyGridMapMessage),
    pose: PropTypes.oneOfType([
      PropTypes.instanceOf(PosefWithTimestampMessage),
      PropTypes.instanceOf(Pose3fWithTimestampMessage),
    ]),
    pointcloudFrame: PropTypes.instanceOf(PointcloudFrameMessage),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    occupancyGridMap: null,
    pose: null,
    pointcloudFrame: null,
  };

  componentDidMount() {
    const { engine, scene } = createScene(this.canvas);
    this.scene = scene;

    const camera = new ArcRotateCamera('main-scene-camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, -30, 30);
    camera.attachControl(this.canvas, true);
    this.camera = camera;

    const light = new HemisphericLight('main-scene-light', new Vector3(0, 0, 1), scene);
    light.intensity = 0.7;

    const material = new GridMaterial('main-scene-grid-material', scene);
    material.mainColor = backgroundColor();
    material.opacity = 0.8;

    const plane = Mesh.CreatePlane('main-scene-plane', 10, scene, false, Mesh.DOUBLESIDE);
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
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { occupancyGridMap, pose, pointcloudFrame } = this.props;
    let updated = false;
    if (occupancyGridMap !== nextProps.occupancyGridMap) {
      if (!this.occupancyGridMap) {
        const { width, height } = occupancyGridMap.size;
        this.occupancyGridMap = new OccupancyGridMap(
          new OccupancyGridMapWorker(),
          width,
          height,
          this.scene
        );
      }
      this.occupancyGridMap.update(nextProps.occupancyGridMap, this.scene);
      updated = true;
    }

    if (pose !== nextProps.pose) {
      if (!this.pose) {
        this.pose = new Pose(1, this.scene);
      }
      this.pose.update(nextProps.pose);
      this.camera.lockedTarget = this.pose.mesh.position;
      this.camera.position = this.pose.mesh
        .getPositionExpressedInLocalSpace()
        .add(new Vector3(0, 0, 100));

      updated = true;
    }

    if (pointcloudFrame !== nextProps.pointcloudFrame) {
      if (!this.pointcloud) {
        this.pointcloud = new Pointcloud(new PointcloudFrameWorker());
      }
      this.pointcloud.update(nextProps.pointcloudFrame, this.scene);
      updated = true;
    }

    if (updated) return true;

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    if (this.occupancyGridMap) {
      this.occupancyGridMap.terminateWorker();
    }

    if (this.pointcloud) {
      this.pointcloud.terminateWorker();
    }
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
