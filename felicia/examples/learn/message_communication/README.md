# Message Communication

In this example, you can learn how to declare custom message, and communicate.

```bash
// On 1st shell
python3 scripts/felicia.py run //felicia/core/master/rpc/grpc_server_main
// On 2nd shell
python3 scripts/felicia.py run //felicia/examples/learn:simple_message_publisher
// On 3rd shell
python3 scripts/felicia.py run //felicia/examples/learn:simple_message_subscriber
```