import { observable, action } from 'mobx';

import {
  OCCUPANCY_GRID_MAP_MESSAGE,
  POINTCLOUD_FRAME_MESSAGE,
  POSEF_WITH_TIMESTAMP_MESSAGE,
  POSE3F_WITH_TIMESTAMP_MESSAGE,
} from '@felicia-viz/communication';

import TopicSubscribable from 'store/topic-subscribable';
import { toVector3, toQuaternion } from 'util/babylon-util';
import { PointcloudFrameMessage } from './pointcloud-panel-state';

export class OccupancyGridMapMessage {
  constructor(message) {
    const { data, size, resolution, origin, timestamp } = message.data;
    this.size = size;
    this.resolution = resolution;
    this.origin = origin;
    this.data = data;
    this.timestamp = timestamp;
  }
}

export class PosefWithTimestampMessage {
  constructor(message) {
    const { data } = message;
    const { position, theta, timestamp } = data;
    this.position = toVector3(position);
    this.theta = theta;
    this.timestamp = timestamp;
    this.is3D = false;
  }
}

export class Pose3fWithTimestampMessage {
  constructor(message) {
    const { data } = message;
    const { position, orientation, timestamp } = data;
    this.position = toVector3(position);
    this.orientation = toQuaternion(orientation);
    this.timestamp = timestamp;
    this.is3D = true;
  }
}

export default class MainSceneState extends TopicSubscribable {
  @observable occupancyGridMap = null;

  @observable pointcloudFrame = null;

  @observable pose = null;

  @action update(message) {
    switch (message.type) {
      case OCCUPANCY_GRID_MAP_MESSAGE:
        this.occupancyGridMap = new OccupancyGridMapMessage(message);
        break;
      case POINTCLOUD_FRAME_MESSAGE:
        this.pointcloudFrame = new PointcloudFrameMessage(message);
        break;
      case POSEF_WITH_TIMESTAMP_MESSAGE:
        this.pose = new PosefWithTimestampMessage(message);
        break;
      case POSE3F_WITH_TIMESTAMP_MESSAGE:
        this.pose = new Pose3fWithTimestampMessage(message);
        break;
      default:
        break;
    }
  }

  type = () => {
    return 'MainScene';
  };
}
