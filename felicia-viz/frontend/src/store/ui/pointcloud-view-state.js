import { observable, action } from 'mobx';

import { PointcloudMessage } from 'messages/pointcloud';
import TopicSubscribable from 'store/topic-subscribable';

export default class PointcloudViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new PointcloudMessage(message.data);
  }

  viewType = () => {
    return 'PointcloudView';
  };
}
