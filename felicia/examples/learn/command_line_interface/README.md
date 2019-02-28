# Command Line Interface

In this example, you can learn how to use cli so you can query whatever you want from server.

On one shell, let's run server

```bash
python3 scripts/felicia.py run //felicia/core/master/rpc/grpc_server_main
```

On the other shell, now you should build the cli.

```bash
python3 scripts/felicia.py build //felicia/core/master/tool/grpc_master_client_cli
```

And let's begin with `get` command!

```bash
bazel-bin/felicia/core/master/tool/grpc_master_client_cli node get -a
```

