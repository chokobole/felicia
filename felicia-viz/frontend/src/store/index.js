import { observable, action } from 'mobx';

import { UIState } from '@felicia-viz/ui';

import TopicInfo from 'store/topic-info';

export class FeliciaVizStore {
  @observable uiState = new UIState();

  @observable topicInfo = new TopicInfo();

  @action update(message) {
    this.topicInfo.update(message);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
