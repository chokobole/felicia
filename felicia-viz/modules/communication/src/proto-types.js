import protobuf from 'protobufjs/light';

import feliciaProtobufJson from './proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);

export const CAMERA_FRAME_MESSAGE = 'felicia.CameraFrameMessage';
export const DEPTH_CAMERA_FRAME_MESSAGE = 'felicia.DepthCameraFrameMessage';
export const IMU_MESSAGE = 'felicia.ImuMessage';
export const TOPIC_INFO = 'felicia.TopicInfo';

const PROTO_TYPES = {};
PROTO_TYPES[CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
PROTO_TYPES[DEPTH_CAMERA_FRAME_MESSAGE] = FeliciaProtoRoot.lookupType(DEPTH_CAMERA_FRAME_MESSAGE);
PROTO_TYPES[IMU_MESSAGE] = FeliciaProtoRoot.lookupType(IMU_MESSAGE);
PROTO_TYPES[TOPIC_INFO] = FeliciaProtoRoot.lookupType(TOPIC_INFO);

export default PROTO_TYPES;
