// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { Message } from '@felicia-viz/communication/subscriber';
import CameraFrameMessage from '@felicia-viz/proto/messages/camera-frame-message';
import { action, observable } from 'mobx';
import TopicSubscribable from '../topic-subscribable';

export default class CameraFrameViewState extends TopicSubscribable {
  @observable frame: CameraFrameMessage | null = null;

  @action update(message: Message): void {
    this.frame = new CameraFrameMessage(message.data);
  }

  viewType = (): string => {
    return 'CameraFrameView';
  };
}
