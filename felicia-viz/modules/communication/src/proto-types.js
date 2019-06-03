import protobuf from 'protobufjs/light';

import feliciaProtobufJson from './proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);

export const CAMERA_FRAME_MESSAGE = 'felicia.CameraFrameMessage';
export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.DepthCameraFrameMessage';
export const IMU_MESSAGE = 'felicia.ImuMessage';
export const TOPIC_INFO = 'felicia.TopicInfo';

const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
const TopicInfo = FeliciaProtoRoot.lookupType(TOPIC_INFO);
export const PixelFormat = FeliciaProtoRoot.lookupEnum('felicia.PixelFormat');

const PROTO_TYPES = {};
PROTO_TYPES[CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
PROTO_TYPES[DEPTH_CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(DEPTH_CAMERA_FRAME_MESSAGE);
PROTO_TYPES[IMU_MESSAGE] = FeliciaProtoRoot.lookupType(IMU_MESSAGE);
PROTO_TYPES[TOPIC_INFO] = TopicInfo;

export function hasWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.some(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'WS';
  });
}

export function findWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.find(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'WS';
  });
}

export default PROTO_TYPES;
