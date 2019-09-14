import { action, observable, toJS } from 'mobx';
import { NotificationManager } from 'react-notifications';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/proto/messages/master-data';

import SUBSCRIBER from 'store/subscriber';

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

    const topics = toJS(this.topics);
    newTopics.forEach(value => {
      if (hasWSChannel(value)) {
        if (!topics.find(topic => topic.topic === value.topic)) {
          NotificationManager.info(`Topic '${value.topic}' was connected.`, 'Topic Info');
          updated = true;
        }
      }
    });

    topics.forEach(value => {
      if (hasWSChannel(value)) {
        if (!newTopics.find(topic => topic.topic === value.topic)) {
          NotificationManager.error(`Topic '${value.topic}' was disconnected.`, 'Topic Info');
          updated = true;
        }
      }
    });

    if (updated || !arraysEqual(this.topics, newTopics)) {
      this.updateTopics(newTopics);
    }
  }
}
