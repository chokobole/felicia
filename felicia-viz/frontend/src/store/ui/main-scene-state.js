import { observable, action } from 'mobx';

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

export default class MainSceneState extends TopicSubscribable {
  @observable map = null;

  @action update(message) {
    this.map = new OccupancyGridMap(message);
  }

  type = () => {
    return 'MainScene';
  };
}
