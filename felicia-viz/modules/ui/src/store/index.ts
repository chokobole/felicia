import { Message } from '@felicia-viz/communication/subscriber';
import { action, observable } from 'mobx';
import TopicInfo from './topic-info';
import UIState from './ui-state';

export class FeliciaVizStore {
  @observable uiState = new UIState();

  @observable topicInfo = new TopicInfo();

  @action update(message: Message): void {
    this.topicInfo.update(message);
    this.uiState.update(message);
  }
}

const STORE = new FeliciaVizStore();

export default STORE;
