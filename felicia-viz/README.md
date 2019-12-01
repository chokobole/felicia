# Felicia-viz

## How to build

```bash
npm install
npm run bootstrap
```

### To build wasm

**On windows it's not supported yet.**

On felicia root, run this command below!

```bash
bazel build --config wasm //felicia/wasm:felicia_wasm
```

Once you failed with the reason below,

```bash
Use --sandbox_debug to see verbose messages from the sandbox
cp: cannot stat 'toolchain/emscripten_cache/*': No such file or directory
```

Then you should run command below. Please refer to section 9 in [bazel tutorial](https://docs.bazel.build/versions/0.24.0/tutorial/cc-toolchain-config.html#configuring-the-c-toolchain).

```bash
./toolchain/embuilder.sh build compiler-rt dlmalloc libc libc-wasm libc++ libc++_noexcept libc++abi pthreads
```

And if you try again, you will find that it succeds.

```bash
bazel build --config wasm //felicia/wasm:felicia_wasm
```

Finally to make use of this build result, you have to run this command below.

```bash
cd felicia-viz && node install-wasm.js
```

### For windows developers

To use felicia-viz, you need a `felicia_js.node` which is built from target `//felicia/js:felicia_js_node`. But on windows, when linking agains `node.lib`, `zlib` and `openssl` inside `node.lib` and `zlib` and `boringssl` inside grpc are conflit each other. So you should build a `felicia_js.node` without `grpc` library. You can do like below.

**Please try not with `-c dbg`, because `node.lib` is provided with only release version.**

```bash
bazel build --define win_no_grpc=true --define framework_shared_object=true //felicia/js:felicia_js_node
```

And then move the `felicia_js.node` to the root directory. This will be used from the backend side.

```bash
mv bazel-bin/felicia/js/felicia_js.node .
```

Lastly you should build again whatever you want, but just removing `--define win_no_grpc=true` and it's done.

## How to run

```bash
cd backend
npm run start:dev
```