import { observable, action } from 'mobx';

import { LidarFrameMessage } from 'messages/lidar-frame';
import TopicSubscribable from 'store/topic-subscribable';

export default class LidarViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new LidarFrameMessage(message.data);
  }

  viewType = () => {
    return 'LidarView';
  };
}
