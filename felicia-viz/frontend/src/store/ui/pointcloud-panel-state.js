import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';

export class PointcloudFrameMessage {
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
    this.frame = new PointcloudFrameMessage(message);
  }

  type = () => {
    return 'PointcloudPanel';
  };
}
