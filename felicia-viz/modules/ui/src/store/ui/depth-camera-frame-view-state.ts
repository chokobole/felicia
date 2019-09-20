import { Message } from '@felicia-viz/communication/subscriber';
import CameraFrameMessage, {
  CAMERA_FRAME_MESSAGE,
} from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { action, observable } from 'mobx';
import TopicSubscribable from '../topic-subscribable';

export default class DepthCameraFrameViewState extends TopicSubscribable {
  @observable frame: DepthCameraFrameMessage | null = null;

  @observable filter = 'jet';

  @observable topicToAlign = '';

  @observable frameToAlign: CameraFrameMessage | null = null;

  @action update(message: Message): void {
    const { type, data } = message;
    if (type === CAMERA_FRAME_MESSAGE) {
      this.frameToAlign = new CameraFrameMessage(data);
    } else {
      this.frame = new DepthCameraFrameMessage(data);
    }
  }

  @action setFilter(newFilter: string): void {
    this.filter = newFilter;
  }

  viewType = (): string => {
    return 'DepthCameraFrameView';
  };
}
