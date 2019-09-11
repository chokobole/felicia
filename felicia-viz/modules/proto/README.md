# Felicia-viz proto

## How to add protobuf

```bash
npx pbjs -t json ../../../felicia/core/protobuf/*.proto ../../../felicia/drivers/**/*.proto ../../../felicia/map/**/*.proto ../../../felicia/slam/**/*.proto > src/felicia_proto_bundle.json
```