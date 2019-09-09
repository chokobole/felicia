/* eslint import/prefer-default-export: "off" */
import { Image } from './image';

export class ImageWithHumansMessage {
  constructor(message) {
    const { image, model, humans } = message;
    this.image = new Image(image);
    this.model = model;
    this.humans = humans;
  }
}
