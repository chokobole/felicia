import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';

export class PointcloudFrame {
  constructor(message) {
    const { points, colors, timestamp } = message.data;
    this.points = points;
    this.colors = colors;
    this.timestamp = timestamp;
  }
}

export default class PointcloudPanelState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new PointcloudFrame(message);
  }

  type = () => {
    return 'PointcloudPanel';
  };
}
