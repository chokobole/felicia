import { Message } from '@felicia-viz/communication/subscriber';
import {
  hasWSChannel,
  TopicInfoProtobuf,
  TOPIC_INFO,
} from '@felicia-viz/proto/messages/master-data';
import { action, observable, toJS } from 'mobx';
// @ts-ignore
import { NotificationManager } from 'react-notifications';
// @ts-ignore
import { shallowEqualArrays } from 'shallow-equal';
import SUBSCRIBER from './subscriber';

export default class TopicInfo {
  @observable topics: Array<TopicInfoProtobuf> = [];

  @action updateTopics(newTopics: Array<TopicInfoProtobuf>): void {
    this.topics = newTopics;

    SUBSCRIBER.updateTopics(newTopics);
  }

  update(message: Message): void {
    if (TOPIC_INFO !== message.type) return;

    const newTopics: Array<TopicInfoProtobuf> = message.data;
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
