# Felicia-viz

## How to build

```bash
npm install
npx run bootstrap
```

### For windows developers

To use felicia-viz, you need a `felicia_js.node` which is built from target `//felicia/js:felicia_js_node`. But on windows, when linking agains `node.lib`, `zlib` and `openssl` inside `node.lib` and `zlib` and `boringssl` inside grpc are conflit each other. So you should build a `felicia_js.node` without `grpc` library. You can do like below.

**Please try not with `-c dbg`, because `node.lib` is provided with only release version.**

```bash
bazel build --config win_no_grpc //felicia/js:felicia_js_node
```

### How to add protobuf

Currently we encoded `CameraFrameMessage` message to `ArrayBuffer` from the backend and decoded it back from the frontend. So you need to let both sides know the type before and can do it with below.

```bash
// at felicia-viz/backend
npx pbjs -t json ../../felicia/drivers/camera/camera_frame_message.proto ../../felicia/drivers/camera/camera_format_message.proto  > ../common/proto_bundle/felicia_proto_bundle.json
```

## How to run

```bash
cd backend
npm run start:dev
```