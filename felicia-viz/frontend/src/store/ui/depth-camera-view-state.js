import { observable, action } from 'mobx';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication/proto-types';

import { CameraFrameMessage } from 'messages/camera-frame';
import { DepthCameraFrameMessage } from 'messages/depth-camera-frame';
import TopicSubscribable from 'store/topic-subscribable';

export default class DepthCameraViewState extends TopicSubscribable {
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
    return 'DepthCameraView';
  };
}
