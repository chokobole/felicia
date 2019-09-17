import { observable, action } from 'mobx';

import {
  POSEF_WITH_TIMESTAMP_MESSAGE,
  POSE3F_WITH_TIMESTAMP_MESSAGE,
  PoseWithTimestampMessage,
  Pose3WithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
import {
  OCCUPANCY_GRID_MAP_MESSAGE,
  POINTCLOUD_MESSAGE,
  OccupancyGridMapMessage,
  PointcloudMessage,
} from '@felicia-viz/proto/messages/map-message';
import TopicSubscribable from '@felicia-viz/ui/store/topic-subscribable';

export class CameraState {
  @observable followPose = false;

  @action setFollowPose(newFollowPose) {
    this.followPose = newFollowPose;
  }
}

export default class MainSceneState extends TopicSubscribable {
  @observable map = null;

  @observable pose = null;

  @observable camera = new CameraState();

  @action update(message) {
    const { type, data } = message;
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

  viewType = () => {
    return 'MainScene';
  };
}
