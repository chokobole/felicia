import { observable, action } from 'mobx';

import { ImageWithBoundingBoxesMessage } from '@felicia-viz/proto/messages/bounding-box';
import TopicSubscribable from '@felicia-viz/ui/store/topic-subscribable';

export default class ImageWithBoundingBoxesViewState extends TopicSubscribable {
  @observable frame = null;

  @observable lineWidth = 10;

  @observable threshold = 0.5;

  @action update(message) {
    this.frame = new ImageWithBoundingBoxesMessage(message.data);
  }

  @action setLineWidth(newLineWidth) {
    this.lineWidth = newLineWidth;
  }

  @action setThreshold(newThreshold) {
    this.threshold = newThreshold;
  }

  viewType = () => {
    return 'ImageWithBoundingBoxesView';
  };
}
