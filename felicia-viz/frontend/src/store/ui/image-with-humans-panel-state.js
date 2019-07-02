import { observable, action } from 'mobx';

import PanelState from './panel-state';

export class ImageWithHumans {
  constructor(message) {
    const { data } = message;
    const { image, model, humans } = data;
    this.image = image;
    this.model = model;
    this.humans = humans;
  }
}

export default class ImageWithHumansPanelState extends PanelState {
  @observable frame = null;

  @observable threshold = 0.3;

  @action update(message) {
    this.frame = new ImageWithHumans(message);
  }

  @action setThreshold(newThreshold) {
    this.threshold = newThreshold;
  }

  type = () => {
    return 'ImageWithHumansPanel';
  };
}
