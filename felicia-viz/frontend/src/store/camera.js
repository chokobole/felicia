import { observable, action } from 'mobx';

export class CameraFrame {
  constructor(frame) {
    const { length, width, height, data } = frame;
    this.length = length;
    this.width = width;
    this.height = height;
    this.data = data;
  }
}

export default class Camera {
  @observable frame = null;

  @action updateFrame(newFrame) {
    this.frame = new CameraFrame(newFrame);
  }

  update(world) {
    this.updateFrame(world.frame);
  }
}
