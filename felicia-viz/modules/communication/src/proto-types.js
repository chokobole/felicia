import protobuf from 'protobufjs/light';

import feliciaProtobufJson from './proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);

export const IMAGE_WITH_BOUNDING_BOXES_MESSAGE = 'felicia.ImageWithBoundingBoxesMessage';
export const IMAGE_WITH_HUMANS_MESSAGE = 'felicia.ImageWithHumansMessage';
export const POSEF_WITH_TIMESTAMP_MESSAGE = 'felicia.PosefWithTimestampMessage';
export const POSE3F_WITH_TIMESTAMP_MESSAGE = 'felicia.Pose3fWithTimestampMessage';
export const TOPIC_INFO = 'felicia.TopicInfo';

// drivers message
export const CAMERA_FRAME_MESSAGE = 'felicia.drivers.CameraFrameMessage';
export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.drivers.DepthCameraFrameMessage';
export const IMU_FRAME_MESSAGE = 'felicia.drivers.ImuFrameMessage';
export const LIDAR_FRAME_MESSAGE = 'felicia.drivers.LidarFrameMessage';

// map message
export const POINTCLOUD_MESSAGE = 'felicia.map.PointcloudMessage';
export const OCCUPANCY_GRID_MAP_MESSAGE = 'felicia.map.OccupancyGridMapMessage';

// enums
export const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
export const DataChannelType = FeliciaProtoRoot.lookupEnum('felicia.DataMessage.ChannelType');
export const DataElementType = FeliciaProtoRoot.lookupEnum('felicia.DataMessage.ElementType');
export const HumanBody = FeliciaProtoRoot.lookupEnum('felicia.HumanBody');
export const HumanBodyModel = FeliciaProtoRoot.lookupEnum('felicia.HumanBodyModel');
export const PixelFormat = FeliciaProtoRoot.lookupEnum('felicia.PixelFormat');

const PROTO_TYPES = {};
PROTO_TYPES[IMAGE_WITH_BOUNDING_BOXES_MESSAGE] = FeliciaProtoRoot.lookupType(
  IMAGE_WITH_BOUNDING_BOXES_MESSAGE
);
PROTO_TYPES[IMAGE_WITH_HUMANS_MESSAGE] = FeliciaProtoRoot.lookupType(IMAGE_WITH_HUMANS_MESSAGE);
PROTO_TYPES[POSEF_WITH_TIMESTAMP_MESSAGE] = FeliciaProtoRoot.lookupType(
  POSEF_WITH_TIMESTAMP_MESSAGE
);
PROTO_TYPES[POSE3F_WITH_TIMESTAMP_MESSAGE] = FeliciaProtoRoot.lookupType(
  POSE3F_WITH_TIMESTAMP_MESSAGE
);
PROTO_TYPES[TOPIC_INFO] = FeliciaProtoRoot.lookupType(TOPIC_INFO);

// drivers message
PROTO_TYPES[CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
PROTO_TYPES[DEPTH_CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(DEPTH_CAMERA_FRAME_MESSAGE);
PROTO_TYPES[IMU_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(IMU_FRAME_MESSAGE);
PROTO_TYPES[LIDAR_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(LIDAR_FRAME_MESSAGE);

// map message
PROTO_TYPES[POINTCLOUD_MESSAGE] = FeliciaProtoRoot.lookupType(POINTCLOUD_MESSAGE);
PROTO_TYPES[OCCUPANCY_GRID_MAP_MESSAGE] = FeliciaProtoRoot.lookupType(OCCUPANCY_GRID_MAP_MESSAGE);

export function findWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.find(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export function hasWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.some(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export default PROTO_TYPES;
