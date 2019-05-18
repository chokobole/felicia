import { observable } from 'mobx';

import UIState from 'store/ui-state';
import MetaInfo from 'store/meta-info';

export class FeliciaVizStore {
  @observable uiState = new UIState();

  @observable metaInfo = new MetaInfo();

  update(message) {
    this.metaInfo.update(message);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
