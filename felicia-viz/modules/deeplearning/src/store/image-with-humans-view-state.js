import { observable, action } from 'mobx';

import { ImageWithHumansMessage } from '@felicia-viz/proto/messages/human';
import { TopicSubscribable } from '@felicia-viz/ui';

export default class ImageWithHumansViewState extends TopicSubscribable {
  @observable frame = null;

  @observable threshold = 0.3;

  @action update(message) {
    this.frame = new ImageWithHumansMessage(message.data);
  }

  @action setThreshold(newThreshold) {
    this.threshold = newThreshold;
  }

  viewType = () => {
    return 'ImageWithHumansView';
  };
}
