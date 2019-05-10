import { observable, action } from 'mobx';

import { UIState } from 'store/ui-state';

class FeliciaVizStore {
  @observable currentTime = 0;

  @observable uiState = new UIState();

  @action updateCurrentTime(newCurrentTime) {
    this.currentTime = newCurrentTime;
  }

  update(message) {
    // this.updateCurrentTime(message.currentTime);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
