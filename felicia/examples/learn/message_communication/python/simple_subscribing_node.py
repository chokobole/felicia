import felicia_py as fel
from felicia.python.node_lifecycle import NodeLifecycle


class SimpleSubscribingNode(NodeLifecycle):
    def __init__(self, topic):
        super().__init__()
        self.topic = topic
        self.subscriber = fel.Subscriber()

    def on_init(self):
        print("SimpleSubscribingNode.on_init()")

    def on_did_create(self, node_info):
        print("SimpleSubscribingNode.on_did_create()")
        self.node_info = node_info
        self.request_subscribe()

    def on_error(self, status):
        print("SimpleSubscribingNode.on_error()")

    def request_subscribe(self):
        settings = fel.Settings()

        self.subscriber.request_subscribe(self.node_info, self.topic, self.on_message,
                                          self.on_subscription_error, settings, self.on_request_subscribe)

    def on_message(self, message):
        print("SimpleSubscribingNode.on_message()")
        print("message : {}".format(message))

    def on_subscription_error(self, status):
        print("SimpleSubscribingNode.on_subscription_error()")
        if not status.ok():
            print(status.error_message())

    def on_request_subscribe(self, status):
        print("SimpleSubscribingNode.on_request_subscribe()")
        if not status.ok():
            print(status.error_message())

    def request_unsubscribe(self):
        self.subscriber.request_unsubscribe(
            self.node_info, self.topic, self.on_request_unsubscribe)

    def on_request_unsubscribe(self, status):
        print("SimpleSubscribingNode.on_request_unsubscribe()")
        if not status.ok():
            print(status.error_message())
