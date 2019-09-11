/* eslint import/prefer-default-export: "off" */
import { RectMessage } from './geometry';
import { Color3uMessage, Image } from './ui';

export const IMAGE_WITH_BOUNDING_BOXES_MESSAGE = 'felicia.ImageWithBoundingBoxesMessage';

export class BoundingBoxMessage {
  constructor(message) {
    const { color, box, label, score } = message;
    this.color = new Color3uMessage(color);
    this.box = new RectMessage(box);
    this.label = label;
    this.score = score;
  }
}

export class ImageWithBoundingBoxesMessage {
  constructor(message) {
    const { image, boundingBoxes } = message;
    this.image = new Image(image);
    this.boundingBoxes = boundingBoxes.map(bb => new BoundingBoxMessage(bb));
  }
}
