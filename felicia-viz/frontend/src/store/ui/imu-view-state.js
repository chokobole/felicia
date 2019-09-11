import { observable, action } from 'mobx';

import ImuFrameMessage from '@felicia-viz/proto/messages/imu-frame-message';

import TopicSubscribable from 'store/topic-subscribable';

export default class ImuViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new ImuFrameMessage(message.data);
  }

  viewType = () => {
    return 'ImuView';
  };
}
