import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';

export class ImageWithHumans {
  constructor(message) {
    const { image, model, humans } = message.data;
    this.image = image;
    this.model = model;
    this.humans = humans;
  }
}

export default class ImageWithHumansPanelState extends TopicSubscribable {
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
