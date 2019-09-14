import { observable, action } from 'mobx';

import LidarFrameMessage from '@felicia-viz/proto/messages/lidar-frame-message';

import TopicSubscribable from '../topic-subscribable';

export default class LidarFrameViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new LidarFrameMessage(message.data);
  }

  viewType = () => {
    return 'LidarFrameView';
  };
}
