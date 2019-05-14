/* eslint import/no-unresolved: ["error", { ignore: ['protobufjs/light'] }] */
import protobuf from 'protobufjs/light';

import feliciaProtobufJson from 'common/proto_bundle/felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
export const CAMERA_FRAME_MESSAGE = 'felicia.CameraFrameMessage';
export const CameraFrameMessage = FeliciaProtoRoot.lookupType(CAMERA_FRAME_MESSAGE);
