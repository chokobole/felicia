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

// var currentTime = 0;

// const timer = setInterval(() => {
//   STORE.update({
//     currentTime: currentTime
//   });
//   currentTime += 1
// }, 100);

export default STORE;
