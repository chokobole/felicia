// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { Message } from '@felicia-viz/communication/subscriber';
import {
  POSE3F_WITH_TIMESTAMP_MESSAGE,
  Pose3WithTimestampMessage,
  POSEF_WITH_TIMESTAMP_MESSAGE,
  PoseWithTimestampMessage,
  Vector3Message,
} from '@felicia-viz/proto/messages/geometry';
import {
  OccupancyGridMapMessage,
  OCCUPANCY_GRID_MAP_MESSAGE,
  PointcloudMessage,
  POINTCLOUD_MESSAGE,
} from '@felicia-viz/proto/messages/map-message';
import TopicSubscribable from '@felicia-viz/ui/store/topic-subscribable';
import { action, observable } from 'mobx';

export class CameraState {
  @observable followPose = false;

  @observable position = new Vector3Message({ x: 0, y: -30, z: 30 });

  @action setFollowPose(newFollowPose: boolean): void {
    this.followPose = newFollowPose;
  }

  @action setPosition(newPosition: Vector3Message): void {
    this.position = newPosition;
  }
}

export default class MainSceneState extends TopicSubscribable {
  @observable map: OccupancyGridMapMessage | PointcloudMessage | null = null;

  @observable pose: PoseWithTimestampMessage | Pose3WithTimestampMessage | null = null;

  @observable camera = new CameraState();

  @action update({ type, data }: Message): void {
    switch (type) {
      case OCCUPANCY_GRID_MAP_MESSAGE:
        this.map = new OccupancyGridMapMessage(data);
        break;
      case POINTCLOUD_MESSAGE:
        this.map = new PointcloudMessage(data);
        break;
      case POSEF_WITH_TIMESTAMP_MESSAGE:
        this.pose = new PoseWithTimestampMessage(data);
        break;
      case POSE3F_WITH_TIMESTAMP_MESSAGE:
        this.pose = new Pose3WithTimestampMessage(data);
        break;
      default:
        break;
    }
  }

  viewType = (): string => {
    return 'MainScene';
  };
}
