import { Message } from '@felicia-viz/communication/subscriber';
import { ImageWithHumansMessage } from '@felicia-viz/proto/messages/human';
import TopicSubscribable from '@felicia-viz/ui/store/topic-subscribable';
import { action, observable } from 'mobx';

export default class ImageWithHumansViewState extends TopicSubscribable {
  @observable frame: ImageWithHumansMessage | null = null;

  @observable threshold = 0.3;

  @action update(message: Message): void {
    this.frame = new ImageWithHumansMessage(message.data);
  }

  @action setThreshold(newThreshold: number): void {
    this.threshold = newThreshold;
  }

  viewType = (): string => {
    return 'ImageWithHumansView';
  };
}
