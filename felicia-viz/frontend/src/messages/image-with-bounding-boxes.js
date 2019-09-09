/* eslint import/prefer-default-export: "off" */
import { Image } from './image';

export class ImageWithBoundingBoxesMessage {
  constructor(message) {
    const { image, boundingBoxes } = message;
    this.image = new Image(image);
    this.boundingBoxes = boundingBoxes;
  }
}
