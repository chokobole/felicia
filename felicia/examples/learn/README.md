# Learn

## Lists
### Message Communication
In this example, you can learn how to declare custom message, and communicate.
  ```/bin/bash
  bazel build //felicia/examples/learn:simple_message_publisher
  bazel-bin/felicia/examples/learn/simple_message_publisher
  bazel build //felicia/examples/learn:simple_message_subscriber
  bazel-bin/felicia/examples/learn/simple_message_subscriber
  ```