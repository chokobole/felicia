import { observable, action } from 'mobx';

import UIState from 'store/ui-state';
import MetaInfo from 'store/meta-info';

export class FeliciaVizStore {
  @observable currentTime = 0;

  @observable uiState = new UIState();

  @observable metaInfo = new MetaInfo();

  @action updateCurrentTime(newCurrentTime) {
    this.currentTime = newCurrentTime;
  }

  update(message) {
    // this.updateCurrentTime(message.currentTime);
    this.metaInfo.update(message);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
