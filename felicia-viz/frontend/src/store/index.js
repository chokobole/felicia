import { observable } from 'mobx';

import UIState from 'store/ui-state';
import TopicInfo from 'store/topic-info';

export class FeliciaVizStore {
  @observable uiState = new UIState();

  @observable topicInfo = new TopicInfo();

  update(message) {
    this.topicInfo.update(message);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
