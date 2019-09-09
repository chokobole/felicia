import { observable, action } from 'mobx';

import { CameraFrameMessage } from 'messages/camera-frame';
import TopicSubscribable from 'store/topic-subscribable';

export default class CameraViewState extends TopicSubscribable {
  @observable frame = null;

  @action update(message) {
    this.frame = new CameraFrameMessage(message.data);
  }

  viewType = () => {
    return 'CameraView';
  };
}
