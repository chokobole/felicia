import { observable, action } from 'mobx';

import ImuFrameMessage from '@felicia-viz/proto/messages/imu-frame-message';

import TopicSubscribable from '../topic-subscribable';

export default class ImuFrameViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new ImuFrameMessage(message.data);
  }

  viewType = () => {
    return 'ImuFrameView';
  };
}
