import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { GridMaterial } from '@babylonjs/materials/grid';
import { Vector3 } from '@babylonjs/core/Maths/math';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import '@babylonjs/core/Meshes/meshBuilder';

import {
  PoseWithTimestampMessage,
  Pose3WithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
import {
  OccupancyGridMapMessage,
  PointcloudMessage,
} from '@felicia-viz/proto/messages/map-message';
import OccupancyGridMap from '@felicia-viz/ui/messages/occupancy-grid-map';
import Pointcloud from '@felicia-viz/ui/messages/pointcloud';
import Pose from '@felicia-viz/ui/messages/pose';
import OccupancyGridMapWorker from '@felicia-viz/ui/webworkers/occupancy-grid-map-webworker';
import PointcloudWorker from '@felicia-viz/ui/webworkers/pointcloud-webworker';
import { backgroundColor, createScene } from '@felicia-viz/ui/util/babylon-util';

export default class MainScene extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    map: PropTypes.oneOfType([
      PropTypes.instanceOf(OccupancyGridMapMessage),
      PropTypes.instanceOf(PointcloudMessage),
    ]),
    pose: PropTypes.oneOfType([
      PropTypes.instanceOf(PoseWithTimestampMessage),
      PropTypes.instanceOf(Pose3WithTimestampMessage),
    ]),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    map: null,
    pose: null,
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

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { map, pose } = this.props;
    let updated = false;
    if (map !== nextProps.map) {
      if (map instanceof OccupancyGridMapMessage) {
        if (!this.map || !(this.map instanceof OccupancyGridMap)) {
          if (this.map) {
            this.map.terminateWorker();
          }
          const { width, height } = nextProps.map.size;
          this.map = new OccupancyGridMap(new OccupancyGridMapWorker(), width, height, this.scene);
        }
        this.map.update(nextProps.map, this.scene);
      } else if (map instanceof PointcloudMessage) {
        if (!this.map || !(this.map instanceof Pointcloud)) {
          if (this.map) {
            this.map.terminateWorker();
          }
          this.map = new Pointcloud(new PointcloudWorker());
        }
        this.map.update(nextProps.map, this.scene);
      }
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

    if (updated) return true;

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    if (this.map) {
      this.map.terminateWorker();
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
