import { observable, action } from 'mobx';

import Camera from 'store/camera';

class FeliciaVizStore {
  @observable currentTime = 0;

  @observable camera = new Camera();

  @action updateCurrentTime(newCurrentTime) {
    this.currentTime = newCurrentTime;
  }

  update(world) {
    this.updateCurrentTime(world.currentTime);
    this.camera.update(world);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
