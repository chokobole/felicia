/* eslint import/prefer-default-export: "off" */
import FeliciaProtoRoot from '../felicia-proto-root';
import { Image } from './ui';

export const IMAGE_WITH_HUMANS_MESSAGE = 'felicia.ImageWithHumansMessage';

export const HumanBody = FeliciaProtoRoot.lookupEnum('felicia.HumanBody');
export const HumanBodyModel = FeliciaProtoRoot.lookupEnum('felicia.HumanBodyModel');

export class ImageWithHumansMessage {
  constructor(message) {
    const { image, model, humans } = message;
    this.image = new Image(image);
    this.model = model;
    this.humans = humans;
  }
}
