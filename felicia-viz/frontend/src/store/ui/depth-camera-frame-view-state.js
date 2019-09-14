import { observable, action } from 'mobx';

import CameraFrameMessage, {
  CAMERA_FRAME_MESSAGE,
} from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage from '@felicia-viz/proto/messages/depth-camera-frame-message';

import TopicSubscribable from 'store/topic-subscribable';

export default class DepthCameraFrameViewState extends TopicSubscribable {
  @observable frame = null;

  @observable filter = 'jet';

  @observable topicToAlign = '';

  @observable frameToAlign = null;

  @action update(message) {
    const { type, data } = message;
    if (type === CAMERA_FRAME_MESSAGE) {
      this.frameToAlign = new CameraFrameMessage(data);
    } else {
      this.frame = new DepthCameraFrameMessage(data);
    }
  }

  @action setFilter(newFilter) {
    this.filter = newFilter;
  }

  viewType = () => {
    return 'DepthCameraFrameView';
  };
}
