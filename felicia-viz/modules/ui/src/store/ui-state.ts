import SubscriberPool, { Message } from '@felicia-viz/communication/subscriber';
import {
  hasWSChannel,
  TopicInfoProtobuf,
  TOPIC_INFO,
} from '@felicia-viz/proto/messages/master-data';
import { action, observable } from 'mobx';
import TopicSubscribable from './topic-subscribable';
import { FeliciaVizStore } from './index';

export interface UIType {
  name: string;
  className?: string;
  state?: typeof TopicSubscribable;
  renderView?: (id: number) => JSX.Element;
  renderControlPanel?: (store: FeliciaVizStore) => JSX.Element;
}

export default class UIState {
  @observable viewStates: Array<TopicSubscribable> = [];

  @observable activeId = 0;

  @observable controlPanelType: string | null = null;

  id = 0;

  uiTypes: Array<UIType> = [];

  subscriber: SubscriberPool | null = null;

  init(
    uiTypes: { [key: string]: UIType },
    mainSceneType: string,
    subscriber: SubscriberPool
  ): void {
    this.uiTypes = Object.values(uiTypes);
    this.subscriber = subscriber;
    this.addView(mainSceneType);
  }

  @action markActive(id: number, controlPanelType: string | null = null): void {
    this.activeId = id;
    this.controlPanelType = controlPanelType;
  }

  @action markInactive(): void {
    if (this.activeId === 0) return;
    this.removeView(this.activeId);

    this.activeId = 0;
    this.controlPanelType = null;
  }

  @action addView(type: string): void {
    // eslint-disable-next-line no-restricted-syntax
    for (const uiType of this.uiTypes) {
      if (type === uiType.name) {
        this.viewStates.push(new uiType.state!(this.id, this.subscriber!));
        this.markActive(this.id, uiType.renderControlPanel ? type : null);
        this.id += 1;
        break;
      }
    }
  }

  @action removeView(id: number): void {
    const idx = this.viewStates.findIndex(viewState => {
      return viewState.id === id;
    });
    if (idx > -1) this.viewStates.splice(idx, 1);
  }

  @action setControlPanel(type: string | null): void {
    this.controlPanelType = type;
  }

  @action unsetControlPanel(): void {
    this.controlPanelType = null;
  }

  getActiveViewState(): TopicSubscribable | undefined {
    return this.findView(this.activeId);
  }

  findView(id: number): TopicSubscribable | undefined {
    return this.viewStates.find(viewState => {
      return viewState.id === id;
    });
  }

  update(message: Message): void {
    if (TOPIC_INFO === message.type) {
      const newTopics: Array<TopicInfoProtobuf> = message.data;
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
