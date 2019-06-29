import { observable, action } from 'mobx';

import PanelState from './panel-state';

export class ImageWithBoundingBoxes {
  constructor(message) {
    const { data } = message;
    const { image, boundingBoxes } = data;
    this.image = image;
    this.boundingBoxes = boundingBoxes;
  }
}

export default class ImageWithBoundingBoxesPanelState extends PanelState {
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
