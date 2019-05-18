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

## How to run

```bash
cd backend
npm run start:dev
```