import { observable, action } from 'mobx';

import TopicSubscribable from 'store/topic-subscribable';

export class ImageWithBoundingBoxes {
  constructor(message) {
    const { image, boundingBoxes } = message.data;
    this.image = image;
    this.boundingBoxes = boundingBoxes;
  }
}

export default class ImageWithBoundingBoxesPanelState extends TopicSubscribable {
  @observable frame = null;

  @observable lineWidth = 10;

  @observable threshold = 0.5;

  @action update(message) {
    this.frame = new ImageWithBoundingBoxes(message);
  }

  @action setLineWidth(newLineWidth) {
    this.lineWidth = newLineWidth;
  }

  @action setThreshold(newThreshold) {
    this.threshold = newThreshold;
  }

  type = () => {
    return 'ImageWithBoundingBoxesPanel';
  };
}
