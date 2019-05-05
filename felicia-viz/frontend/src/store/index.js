import { observable, action } from 'mobx';

import Camera from 'store/camera';

class FeliciaVizStore {
  @observable currentTime = 0;

  @observable camera = new Camera();

  @action updateCurrentTime(newCurrentTime) {
    this.currentTime = newCurrentTime;
  }

  update(message) {
    // this.updateCurrentTime(message.currentTime);
    if (message.type === 'Camera') {
      this.camera.update(message);
    }
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
