import { action, observable } from 'mobx';
import { NotificationManager } from 'react-notifications';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/communication';

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
    this.topics = newTopics;

    SUBSCRIBER.updateTopics(newTopics);
  }

  update(message) {
    if (TOPIC_INFO !== message.type) return;

    const newTopics = message.data;
    let updated = false;

    newTopics.forEach(value => {
      if (hasWSChannel(value)) {
        if (!this.topics.find(topic => topic === value)) {
          NotificationManager.info(`Topic '${value.topic}' is being published.`, 'Topic Info');
          updated = true;
        }
      }
    });

    if (updated || !arraysEqual(this.topics, newTopics)) {
      this.updateTopics(newTopics);
    }
  }
}
