import protobuf from 'protobufjs/light';

import feliciaProtobufJson from './proto_bundle/felicia_proto_bundle.json';

export const CAMERA_FRAME_MESSAGE = 'felicia.CameraFrameMessage';
export const IMU_MESSAGE = 'felicia.ImuMessage';
export const TOPIC_INFO = 'felicia.TopicInfo';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
const CameraFrameMessage = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
const ImuMessage = FeliciaProtoRoot.lookupType(IMU_MESSAGE);
const TopicInfo = FeliciaProtoRoot.lookupType(TOPIC_INFO);

const PROTO_TYPES = {};
PROTO_TYPES[CAMERA_FRAME_MESSAGE] = CameraFrameMessage;
PROTO_TYPES[IMU_MESSAGE] = ImuMessage;
PROTO_TYPES[TOPIC_INFO] = TopicInfo;

export default PROTO_TYPES;
