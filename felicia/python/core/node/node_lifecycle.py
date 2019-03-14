from felicia.python.core.node.node import _NodeLifecycle

class NodeLifecycle(_NodeLifecycle):
    def OnInit(self):
        print("OnInit called")
        self.on_init()

    def on_init(self):
        pass

    def OnDidCreate(self, node_info):
        print("OnDidCreate called")
        self.on_did_create(node_info)

    def on_did_create(self, node_info):
        pass

    def OnError(self, status):
        self.on_error(status)

    def on_error(self, status):
        pass