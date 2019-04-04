import { observable, action } from 'mobx';

class FeliciaVizStore {
  @observable currentTime = 0;

  @action updateCurrentTime(newCurrentTime) {
    this.currentTime = newCurrentTime;
  }

  update(world) {
    this.updateCurrentTime(world.currentTime);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
