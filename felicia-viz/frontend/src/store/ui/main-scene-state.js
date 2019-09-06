import { observable, action } from 'mobx';

import {
  OCCUPANCY_GRID_MAP_MESSAGE,
  POSEF_WITH_TIMESTAMP_MESSAGE,
} from '@felicia-viz/communication';

import TopicSubscribable from 'store/topic-subscribable';

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

export class PoseWithTimestampMessage {
  constructor(message) {
    const { data } = message;
    const { point, theta, timestamp } = data;
    this.point = point;
    this.theta = theta;
    this.timestamp = timestamp;
  }
}

export default class MainSceneState extends TopicSubscribable {
  @observable occupancyGridMap = null;

  @observable pose = null;

  @action update(message) {
    switch (message.type) {
      case OCCUPANCY_GRID_MAP_MESSAGE:
        this.occupancyGridMap = new OccupancyGridMapMessage(message);
        break;
      case POSEF_WITH_TIMESTAMP_MESSAGE:
        this.pose = new PoseWithTimestampMessage(message);
        break;
      default:
        break;
    }
  }

  type = () => {
    return 'MainScene';
  };
}
