import protobuf from 'protobufjs/light';
import feliciaProtobufJson from './felicia_proto_bundle.json';

const FeliciaProtoRoot = protobuf.Root.fromJSON(feliciaProtobufJson);
export default FeliciaProtoRoot;
