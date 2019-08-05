import { observable, action } from 'mobx';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/communication';

import UI_TYPES, { MainSceneType } from 'store/ui/ui-types';
import SUBSCRIBER from 'util/subscriber';

class ViewState {
  @observable id = null;

  @observable type = null;

  constructor(uiState) {
    this.uiState = uiState;
  }

  @action reset() {
    this.id = null;
    this.type = null;
  }

  @action set(id, type) {
    this.id = id;
    this.type = type;
  }

  @action unset() {
    const { type, id, uiState } = this;
    if (type === null) return;

    const viewState = uiState.findView(id);
    viewState.topics.forEach(topic => {
      SUBSCRIBER.unsubscribeTopic(id, topic);
    });
    uiState.removeView(id);

    this.reset();
  }

  getState() {
    if (this.type === null) return null;

    return this.uiState.findView(this.id);
  }
}

export default class UIState {
  @observable viewStates = [];

  @observable activeViewState = new ViewState(this);

  id = 0;

  constructor() {
    this.addView(MainSceneType.name);
  }

  @action addView(type) {
    const values = Object.values(UI_TYPES);
    // eslint-disable-next-line no-restricted-syntax
    for (const value of values) {
      if (type === value.name) {
        // eslint-disable-next-line new-cap
        this.viewStates.push(new value.state(this.id));
        this.activeViewState.set(this.id, type);
        this.id += 1;
        break;
      }
    }
  }

  @action removeView(id) {
    const idx = this.viewStates.findIndex(viewState => {
      return viewState.id === id;
    });
    if (idx > -1) this.viewStates.splice(idx, 1);
  }

  findView(id) {
    return this.viewStates.find(viewState => {
      return viewState.id === id;
    });
  }

  update(message) {
    if (TOPIC_INFO === message.type) {
      const newTopics = message.data;
      this.viewStates.forEach(viewState => {
        viewState.topics.forEach((topic, typeName) => {
          let found = false;
          newTopics.forEach(value => {
            found = hasWSChannel(value);
          });

          if (!found) {
            viewState.unsetTopic(typeName, topic);
          }
        });
      });
      return;
    }

    this.viewStates.forEach(viewState => {
      if (viewState.id === message.id) {
        viewState.update(message);
      }
    });
  }
}
