import { action, observable } from 'mobx';

import { TOPIC_INFO } from '@felicia-viz/communication';

import SUBSCRIBER from 'util/subscriber';

// TODO: Maybe we need loadash?
function arraysEqual(a, b) {
  if (a === b) return true;
  if (a === null || b === null) return false;
  if (a.length !== b.length) return false;

  for (let i = 0; i < a.length; i += 1) {
    if (a[i] !== b[i]) return false;
  }
  return true;
}

export default class TopicInfo {
  @observable topics = [];

  @action updateTopics(newTopics) {
    console.log('updateTopics');
    this.topics = newTopics;

    SUBSCRIBER.updateTopics(newTopics);
  }

  update(message) {
    if (TOPIC_INFO !== message.type) return;

    const newTopics = message.data;

    if (!arraysEqual(this.topics, newTopics)) {
      this.updateTopics(newTopics);
    }
  }
}
