import { action, observable, toJS } from 'mobx';
import { NotificationManager } from 'react-notifications';
import { shallowEqualArrays } from 'shallow-equal';

import { TOPIC_INFO, hasWSChannel } from '@felicia-viz/proto/messages/master-data';

import SUBSCRIBER from './subscriber';

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

    if (updated || !shallowEqualArrays(this.topics, newTopics)) {
      this.updateTopics(newTopics);
    }
  }
}
