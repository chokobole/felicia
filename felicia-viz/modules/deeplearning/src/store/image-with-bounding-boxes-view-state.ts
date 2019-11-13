// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { Message } from '@felicia-viz/communication/subscriber';
import { ImageWithBoundingBoxesMessage } from '@felicia-viz/proto/messages/bounding-box';
import TopicSubscribable from '@felicia-viz/ui/store/topic-subscribable';
import { action, observable } from 'mobx';

export default class ImageWithBoundingBoxesViewState extends TopicSubscribable {
  @observable frame: ImageWithBoundingBoxesMessage | null = null;

  @observable lineWidth = 10;

  @observable threshold = 0.5;

  @action update(message: Message): void {
    this.frame = new ImageWithBoundingBoxesMessage(message.data);
  }

  @action setLineWidth(newLineWidth: number): void {
    this.lineWidth = newLineWidth;
  }

  @action setThreshold(newThreshold: number): void {
    this.threshold = newThreshold;
  }

  viewType = (): string => {
    return 'ImageWithBoundingBoxesView';
  };
}
