import { observable, action } from 'mobx';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/proto/messages/master-data';

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
    // Don't unset MainScene
    if (id === 0) return;

    const viewState = uiState.findView(id);
    viewState.clear();
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

  init(uiTypes, mainSceneType, subscriber) {
    this.uiTypes = Object.values(uiTypes);
    this.subscriber = subscriber;
    this.addView(mainSceneType);
  }

  @action addView(type) {
    // eslint-disable-next-line no-restricted-syntax
    for (const uiType of this.uiTypes) {
      if (type === uiType.name) {
        // eslint-disable-next-line new-cap
        this.viewStates.push(new uiType.state(this.id, this.subscriber));
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
