import { observable, action } from 'mobx';

import {
  OCCUPANCY_GRID_MAP_MESSAGE,
  POSEF_WITH_TIMESTAMP_MESSAGE,
} from '@felicia-viz/communication';

import TopicSubscribable from 'store/topic-subscribable';

export class OccupancyGridMap {
  constructor(message) {
    const { data } = message;
    const { size, resolution, origin, timestamp } = data;
    this.size = size;
    this.resolution = resolution;
    this.origin = origin;
    this.data = data.data;
    this.timestamp = timestamp;
  }
}

export class PoseWithTimestamp {
  constructor(message) {
    const { data } = message;
    const { point, theta, timestamp } = data;
    this.point = point;
    this.theta = theta;
    this.timestamp = timestamp;
  }
}

export default class MainSceneState extends TopicSubscribable {
  @observable map = null;

  @observable pose = null;

  @action update(message) {
    if (message.type === OCCUPANCY_GRID_MAP_MESSAGE) {
      this.map = new OccupancyGridMap(message);
    } else if (message.type === POSEF_WITH_TIMESTAMP_MESSAGE) {
      this.pose = new PoseWithTimestamp(message);
    }
  }

  type = () => {
    return 'MainScene';
  };
}
