import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { HemisphericLight } from '@babylonjs/core/Lights/hemisphericLight';
import { Vector3 } from '@babylonjs/core/Maths/math';
import { Mesh } from '@babylonjs/core/Meshes/mesh';
import '@babylonjs/core/Meshes/meshBuilder';
import { Scene } from '@babylonjs/core/scene';
import { GridMaterial } from '@babylonjs/materials/grid';
import {
  Pose3WithTimestampMessage,
  PoseWithTimestampMessage,
  Vector3Message,
} from '@felicia-viz/proto/messages/geometry';
import {
  OccupancyGridMapMessage,
  PointcloudMessage,
} from '@felicia-viz/proto/messages/map-message';
import OccupancyGridMap from '@felicia-viz/ui/messages/occupancy-grid-map';
import Pointcloud from '@felicia-viz/ui/messages/pointcloud';
import Pose from '@felicia-viz/ui/messages/pose';
import { backgroundColor3, createScene } from '@felicia-viz/ui/util/babylon-util';
import OccupancyGridMapWorker from '@felicia-viz/ui/webworkers/occupancy-grid-map-webworker';
import PointcloudWorker from '@felicia-viz/ui/webworkers/pointcloud-webworker';
import React, { Component } from 'react';

export interface Props {
  width?: string;
  height?: string;
  followPose: boolean;
  cameraPosition: Vector3Message;
  map: OccupancyGridMapMessage | PointcloudMessage | null;
  pose: PoseWithTimestampMessage | Pose3WithTimestampMessage | null;
}

export default class MainScene extends Component<Props> {
  static defaultProps = {
    width: '100%',
    height: '100%',
    map: null,
    pose: null,
  };

  private map: OccupancyGridMap | Pointcloud | null = null;

  private pose: Pose | null = null;

  componentDidMount(): void {
    if (!this.canvas) return;
    const { engine, scene } = createScene(this.canvas);
    this.scene = scene;

    const camera = new ArcRotateCamera('main-scene-camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, -30, 30);
    camera.attachControl(this.canvas, true);
    this.camera = camera;

    const light = new HemisphericLight('main-scene-light', new Vector3(0, 0, 1), scene);
    light.intensity = 0.7;

    const material = new GridMaterial('main-scene-grid-material', scene);
    material.mainColor = backgroundColor3();
    material.opacity = 0.8;

    const plane = Mesh.CreatePlane('main-scene-plane', 10, scene, false, Mesh.DOUBLESIDE);
    plane.material = material;

    engine.runRenderLoop(() => {
      const { followPose, cameraPosition } = this.props;
      if (followPose && this.pose) {
        this.camera!.lockedTarget = this.pose.mesh;
        this.camera!.position = this.pose.mesh.position.add(cameraPosition.toBabylonVector3());
      }
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps: Props): boolean {
    if (!this.scene) return false;

    const { followPose, map, pose } = this.props;
    let updated = false;
    if (map !== nextProps.map) {
      if (map instanceof OccupancyGridMapMessage) {
        const occupancyGridMapMessage = nextProps.map as OccupancyGridMapMessage;
        if (!this.map || !(this.map instanceof OccupancyGridMap)) {
          if (this.map) {
            this.map.terminateWorker();
          }
          const { width, height } = occupancyGridMapMessage.size;
          this.map = new OccupancyGridMap(new OccupancyGridMapWorker(), width, height, this.scene);
        }
        (this.map! as OccupancyGridMap).update(occupancyGridMapMessage, this.scene);
      } else if (map instanceof PointcloudMessage) {
        const pointcloudMessage = nextProps.map as PointcloudMessage;
        if (!this.map || !(this.map instanceof Pointcloud)) {
          if (this.map) {
            this.map.terminateWorker();
          }
          this.map = new Pointcloud(new PointcloudWorker());
        }
        (this.map! as Pointcloud).update(pointcloudMessage, this.scene);
      }
      updated = true;
    }

    if (pose !== nextProps.pose) {
      if (!this.pose) {
        this.pose = new Pose(1, this.scene);
      }
      this.pose!.update(nextProps.pose);
      updated = true;
    }

    if (followPose !== nextProps.followPose) {
      if (!nextProps.followPose) {
        this.camera!.lockedTarget = null;
        this.camera!.position = new Vector3(0, -30, 30);
      }
    }

    if (updated) return true;

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  componentWillUnmount(): void {
    if (this.map) {
      this.map.terminateWorker();
    }
  }

  _onCanvasLoad = (ref: HTMLCanvasElement): void => {
    this.canvas = ref;
  };

  private canvas?: HTMLCanvasElement;

  private scene?: Scene;

  private camera?: ArcRotateCamera;

  render(): JSX.Element {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return <canvas style={style} ref={this._onCanvasLoad} />;
  }
}
