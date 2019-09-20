import { Message } from '@felicia-viz/communication/subscriber';
import LidarFrameMessage from '@felicia-viz/proto/messages/lidar-frame-message';
import { action, observable } from 'mobx';
import TopicSubscribable from '../topic-subscribable';

export default class LidarFrameViewState extends TopicSubscribable {
  @observable frame: LidarFrameMessage | null = null;

  @action update(message: Message): void {
    this.frame = new LidarFrameMessage(message.data);
  }

  viewType = (): string => {
    return 'LidarFrameView';
  };
}
