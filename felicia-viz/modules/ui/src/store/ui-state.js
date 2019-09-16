import { observable, action } from 'mobx';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/proto/messages/master-data';

export default class UIState {
  @observable viewStates = [];

  @observable activeId = null;

  @observable controlPanelType = null;

  id = 0;

  init(uiTypes, mainSceneType, subscriber) {
    this.uiTypes = Object.values(uiTypes);
    this.subscriber = subscriber;
    this.addView(mainSceneType);
  }

  @action markActive(id, controlPanelType) {
    this.activeId = id;
    this.controlPanelType = controlPanelType;
  }

  @action markInactive() {
    if (this.activeId === 0) return;
    this.removeView(this.activeId);

    this.activeId = 0;
    this.controlPanelType = null;
  }

  @action addView(type) {
    // eslint-disable-next-line no-restricted-syntax
    for (const uiType of this.uiTypes) {
      if (type === uiType.name) {
        // eslint-disable-next-line new-cap
        this.viewStates.push(new uiType.state(this.id, this.subscriber));
        this.markActive(this.id, uiType.renderControlPanel ? type : null);
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

  @action setControlPanel(type) {
    this.controlPanelType = type;
  }

  @action unsetControlPanel() {
    this.controlPanelType = null;
  }

  getActiveViewState() {
    return this.findView(this.activeId);
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
