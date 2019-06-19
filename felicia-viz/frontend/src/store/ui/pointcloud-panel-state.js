import { observable, action } from 'mobx';

import PanelState from './panel-state';

export class PointcloudFrame {
  constructor(message) {
    const { data } = message;
    const { points, colors, timestamp } = data;
    this.points = points;
    this.colors = colors;
    this.timestamp = timestamp;
  }
}

export default class PointcloudPanelState extends PanelState {
  @observable frame = null;

  @action update(message) {
    this.frame = new PointcloudFrame(message);
  }

  type = () => {
    return 'PointcloudPanel';
  };
}
