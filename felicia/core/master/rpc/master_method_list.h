// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Method, method, cancelable
MASTER_METHOD(RegisterClient, registerClient, true)
MASTER_METHOD(ListClients, listClients, false)
MASTER_METHOD(RegisterNode, registerNode, true)
MASTER_METHOD(UnregisterNode, unregisterNode, true)
MASTER_METHOD(ListNodes, listNodes, false)
MASTER_METHOD(PublishTopic, publishTopic, true)
MASTER_METHOD(UnpublishTopic, unpublishTopic, true)
MASTER_METHOD(SubscribeTopic, subscribeTopic, true)
MASTER_METHOD(UnsubscribeTopic, unsubscribeTopic, true)
MASTER_METHOD(ListTopics, listTopics, false)
MASTER_METHOD(RegisterServiceClient, registerServiceClient, true)
MASTER_METHOD(UnregisterServiceClient, unregisterServiceClient, true)
MASTER_METHOD(RegisterServiceServer, registerServiceServer, true)
MASTER_METHOD(UnregisterServiceServer, unregisterServiceServer, true)
MASTER_METHOD(ListServices, listServices, false)