import protobuf from 'protobufjs/light';

import feliciaProtobufJson from './proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);

export const CAMERA_FRAME_MESSAGE = 'felicia.CameraFrameMessage';
export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.DepthCameraFrameMessage';
export const IMAGE_WITH_BOUNDING_BOXES_MESSAGE = 'felicia.ImageWithBoundingBoxesMessage';
export const IMAGE_WITH_HUMANS_MESSAGE = 'felicia.ImageWithHumansMessage';
export const IMU_FRAME_MESSAGE = 'felicia.ImuFrameMessage';
export const LIDAR_FRAME_MESSAGE = 'felicia.LidarFrameMessage';
export const POINTCLOUD_FRAME_MESSAGE = 'felicia.PointcloudFrameMessage';
export const TOPIC_INFO = 'felicia.TopicInfo';

export const OCCUPANCY_GRID_MAP_MESSAGE = 'felicia.OccupancyGridMapMessage';
export const POSEF_WITH_TIMESTAMP_MESSAGE = 'felicia.PosefWithTimestampMessage';

const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
const TopicInfo = FeliciaProtoRoot.lookupType(TOPIC_INFO);
export const PixelFormat = FeliciaProtoRoot.lookupEnum('felicia.PixelFormat');
export const HumanBody = FeliciaProtoRoot.lookupEnum('felicia.HumanBody');
export const HumanBodyModel = FeliciaProtoRoot.lookupEnum('felicia.HumanBodyModel');

const PROTO_TYPES = {};
PROTO_TYPES[CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
PROTO_TYPES[DEPTH_CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(DEPTH_CAMERA_FRAME_MESSAGE);
PROTO_TYPES[IMAGE_WITH_BOUNDING_BOXES_MESSAGE] = FeliciaProtoRoot.lookupType(
  IMAGE_WITH_BOUNDING_BOXES_MESSAGE
);
PROTO_TYPES[IMAGE_WITH_HUMANS_MESSAGE] = FeliciaProtoRoot.lookupType(IMAGE_WITH_HUMANS_MESSAGE);
PROTO_TYPES[IMU_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(IMU_FRAME_MESSAGE);
PROTO_TYPES[LIDAR_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(LIDAR_FRAME_MESSAGE);
PROTO_TYPES[POINTCLOUD_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(POINTCLOUD_FRAME_MESSAGE);
PROTO_TYPES[TOPIC_INFO] = TopicInfo;

PROTO_TYPES[OCCUPANCY_GRID_MAP_MESSAGE] = FeliciaProtoRoot.lookupType(OCCUPANCY_GRID_MAP_MESSAGE);
PROTO_TYPES[POSEF_WITH_TIMESTAMP_MESSAGE] = FeliciaProtoRoot.lookupType(
  POSEF_WITH_TIMESTAMP_MESSAGE
);

export function hasWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.some(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export function findWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.find(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export default PROTO_TYPES;
