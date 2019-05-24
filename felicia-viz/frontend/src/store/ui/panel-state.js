import { observable, action } from 'mobx';

import SUBSCRIBER from 'util/subscriber';

export default class PanelState {
  @observable topic = '';

  constructor(id) {
    this.id = id;
  }

  @action selectTopic(typeName, newTopic) {
    this.topic = newTopic;
    SUBSCRIBER.subscribeTopic(this.id, typeName, newTopic);
  }
}
