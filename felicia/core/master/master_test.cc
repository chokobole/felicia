#include "felicia/core/master/master.h"

#include <memory>

#include "gtest/gtest.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/master/errors.h"

namespace felicia {

namespace {

void FillRandomTCPChannelDef(ChannelDef* channel_def) {
  IPEndPoint* ip_endpoint = channel_def->mutable_ip_endpoint();
  ip_endpoint->set_ip(HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4).ToString());
  ip_endpoint->set_port(
      PickRandomPort(channel_def->type() == ChannelDef::CHANNEL_TYPE_TCP));
}

void ExpectOK(std::shared_ptr<base::WaitableEvent> event, const Status& s) {
  EXPECT_TRUE(s.ok());
  LOG_IF(ERROR, !s.ok()) << s;
  event->Signal();
}

#define DEFINE_EXPECTED_ERROR_0(Error)                           \
  void Expect##Error(std::shared_ptr<base::WaitableEvent> event, \
                     const Status& s) {                          \
    Status expected = errors::Error();                           \
    EXPECT_TRUE(s == expected);                                  \
    LOG_IF(ERROR, s != expected) << s;                           \
    event->Signal();                                             \
  }

#define DEFINE_EXPECTED_ERROR_1(Error, Type, name)                          \
  void Expect##Error(std::shared_ptr<base::WaitableEvent> event, Type name, \
                     const Status& s) {                                     \
    Status expected = errors::Error(name);                                  \
    EXPECT_TRUE(s == expected);                                             \
    LOG_IF(ERROR, s != expected) << s;                                      \
    event->Signal();                                                        \
  }

#define DEFINE_EXPECTED_ERROR_2(Error, Type, name, Type2, name2)            \
  void Expect##Error(std::shared_ptr<base::WaitableEvent> event, Type name, \
                     Type2 name2, const Status& s) {                        \
    Status expected = errors::Error(name, name2);                           \
    EXPECT_TRUE(s == expected);                                             \
    LOG_IF(ERROR, s != expected) << s;                                      \
    event->Signal();                                                        \
  }

DEFINE_EXPECTED_ERROR_2(ChannelSourceNotValid, const std::string&, name,
                        const ChannelSource&, channel_source)
DEFINE_EXPECTED_ERROR_0(ClientNotRegistered)
DEFINE_EXPECTED_ERROR_1(NodeNotRegistered, const NodeInfo&, node_info)
DEFINE_EXPECTED_ERROR_1(NodeAlreadyRegistered, const NodeInfo&, node_info)
DEFINE_EXPECTED_ERROR_2(TopicAlreadyPublishingOnNode, const NodeInfo&,
                        node_info, const TopicInfo&, topic_info)
DEFINE_EXPECTED_ERROR_2(TopicNotPublishingOnNode, const NodeInfo&, node_info,
                        const std::string&, topic)
DEFINE_EXPECTED_ERROR_2(TopicAlreadySubscribingOnNode, const NodeInfo&,
                        node_info, const std::string&, topic)
DEFINE_EXPECTED_ERROR_2(TopicNotSubscribingOnNode, const NodeInfo&, node_info,
                        const std::string&, topic)
DEFINE_EXPECTED_ERROR_2(ServiceAlreadyRequestingOnNode, const NodeInfo&,
                        node_info, const std::string&, service)
DEFINE_EXPECTED_ERROR_2(ServiceNotRequestingOnNode, const NodeInfo&, node_info,
                        const std::string&, service)
DEFINE_EXPECTED_ERROR_2(ServiceAlreadyServingOnNode, const NodeInfo&, node_info,
                        const ServiceInfo&, service_info)
DEFINE_EXPECTED_ERROR_2(ServiceNotServingOnNode, const NodeInfo&, node_info,
                        const std::string&, service)

#undef DEFINE_EXPECTED_ERROR_0
#undef DEFINE_EXPECTED_ERROR_1
#undef DEFINE_EXPECTED_ERROR_2

}  // namespace

#define DECLARE_REQUEST_AND_RESPONSE(Method)          \
  auto request = std::make_unique<Method##Request>(); \
  auto response = std::make_unique<Method##Response>()

class MasterTest : public testing::Test {
 public:
  MasterTest()
      : master_(base::WrapUnique(new Master())),
        publishing_node_name_("publisher"),
        subscribing_node_name_("subscriber"),
        topic_("topic"),
        client_node_name_("client"),
        server_node_name_("server"),
        service_("service"),
        event_(std::make_shared<base::WaitableEvent>(
            base::WaitableEvent::ResetPolicy::AUTOMATIC,
            base::WaitableEvent::InitialState::NOT_SIGNALED)) {
    master_->SetCheckHeartBeatForTesting(false);
    master_->Run();
  }

  void SetUp() override {
#define REGISTER_NODE(client_id, node_name, node_info) \
  do {                                                 \
    node_info.set_client_id(client_id);                \
    node_info.set_name(node_name);                     \
    DECLARE_REQUEST_AND_RESPONSE(RegisterNode);        \
    *request->mutable_node_info() = node_info;         \
    RegisterNode(request.get(), response.get(),        \
                 base::BindOnce(&ExpectOK, event_));   \
  } while (0)

    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterClient);
      RegisterRandomClientForTesting(request.get(), response.get());
      client_id_ = response->id();
    }

    REGISTER_NODE(client_id_, publishing_node_name_, pub_node_info_);
    {
      DECLARE_REQUEST_AND_RESPONSE(PublishTopic);
      PreparePublishTopic(request.get(), response.get(), pub_node_info_, topic_,
                          &topic_info_);
      PublishTopic(request.get(), response.get(),
                   base::BindOnce(&ExpectOK, event_));
    }

    REGISTER_NODE(client_id_, subscribing_node_name_, sub_node_info_);
    {
      DECLARE_REQUEST_AND_RESPONSE(SubscribeTopic);
      *request->mutable_node_info() = sub_node_info_;
      *request->mutable_topic() = topic_;
      SubscribeTopic(request.get(), response.get(),
                     base::BindOnce(&ExpectOK, event_));
    }

    REGISTER_NODE(client_id_, client_node_name_, client_node_info_);
    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterServiceClient);
      *request->mutable_node_info() = client_node_info_;
      *request->mutable_service() = service_;
      RegisterServiceClient(request.get(), response.get(),
                            base::BindOnce(&ExpectOK, event_));
    }

    REGISTER_NODE(client_id_, server_node_name_, server_node_info_);
    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterServiceServer);
      PrepareRegisterServiceServer(request.get(), response.get(),
                                   server_node_info_, service_, &service_info_);
      RegisterServiceServer(request.get(), response.get(),
                            base::BindOnce(&ExpectOK, event_));
    }
  }

  void TearDown() override {
#define UNREGISTER_NODE(node_info)                     \
  do {                                                 \
    DECLARE_REQUEST_AND_RESPONSE(UnregisterNode);      \
    *request->mutable_node_info() = node_info;         \
    UnregisterNode(request.get(), response.get(),      \
                   base::BindOnce(&ExpectOK, event_)); \
  } while (0)
    UNREGISTER_NODE(pub_node_info_);
    UNREGISTER_NODE(sub_node_info_);
    UNREGISTER_NODE(client_node_info_);
    UNREGISTER_NODE(server_node_info_);
#undef UNREGISTER_NODE
  }

#define MASTER_METHOD(Method, method, cancelable)                   \
  void Method(const Method##Request* arg, Method##Response* result, \
              StatusOnceCallback callback) {                        \
    master_->Method(arg, result, std::move(callback));              \
    event_->Wait();                                                 \
  }
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

  void SetCheckHeartBeatForTesting(bool check_heart_beat) {
    master_->SetCheckHeartBeatForTesting(check_heart_beat);
  }

  void RegisterRandomClientForTesting(RegisterClientRequest* request,
                                      RegisterClientResponse* response) {
    ClientInfo client_info;
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    FillRandomTCPChannelDef(&channel_def);
    ChannelSource master_notification_watcher_source;
    *master_notification_watcher_source.add_channel_defs() = channel_def;
    *client_info.mutable_master_notification_watcher_source() =
        master_notification_watcher_source;
    *request->mutable_client_info() = client_info;
    RegisterClient(request, response, base::BindOnce(&ExpectOK, event_));
  }

  void PreparePublishTopic(PublishTopicRequest* request,
                           PublishTopicResponse* response,
                           const NodeInfo& node_info, const std::string& topic,
                           TopicInfo* topic_info) {
    topic_info->set_topic(topic);
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    FillRandomTCPChannelDef(&channel_def);
    ChannelSource* topic_source = topic_info->mutable_topic_source();
    *topic_source->add_channel_defs() = channel_def;
    *request->mutable_node_info() = node_info;
    *request->mutable_topic_info() = *topic_info;
  }

  void PrepareRegisterServiceServer(RegisterServiceServerRequest* request,
                                    RegisterServiceServerResponse* response,
                                    const NodeInfo& node_info,
                                    const std::string& service,
                                    ServiceInfo* service_info) {
    service_info->set_service(service);
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    FillRandomTCPChannelDef(&channel_def);
    ChannelSource* service_source = service_info->mutable_service_source();
    *service_source->add_channel_defs() = channel_def;
    *request->mutable_node_info() = node_info;
    *request->mutable_service_info() = *service_info;
  }

 protected:
  std::unique_ptr<Master> master_;
  uint32_t client_id_;
  std::string publishing_node_name_;
  std::string subscribing_node_name_;
  std::string topic_;
  std::string client_node_name_;
  std::string server_node_name_;
  std::string service_;
  TopicInfo topic_info_;
  ServiceInfo service_info_;
  NodeInfo pub_node_info_;
  NodeInfo sub_node_info_;
  NodeInfo client_node_info_;
  NodeInfo server_node_info_;
  std::shared_ptr<base::WaitableEvent> event_;
};  // namespace felicia

TEST_F(MasterTest, RegisterClient) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterClient);

  ClientInfo* client_info = request->mutable_client_info();
  SetCheckHeartBeatForTesting(true);

  RegisterClient(request.get(), response.get(),
                 base::BindOnce(&ExpectChannelSourceNotValid, event_,
                                "heart beat signaller",
                                client_info->heart_beat_signaller_source()));
}

namespace {

void OnListAllClients(std::shared_ptr<base::WaitableEvent> event, uint32_t id,
                      ListClientsResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& client_infos = response->client_infos();
  EXPECT_EQ(1, client_infos.size());
  EXPECT_EQ(id, client_infos[0].id());
  event->Signal();
}

void OnListClient(std::shared_ptr<base::WaitableEvent> event, uint32_t id,
                  ListClientsResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& client_infos = response->client_infos();
  EXPECT_EQ(1, client_infos.size());
  EXPECT_EQ(id, client_infos[0].id());
  event->Signal();
}

}  // namespace

TEST_F(MasterTest, ListClients) {
  DECLARE_REQUEST_AND_RESPONSE(ListClients);

  ClientFilter* client_filter = request->mutable_client_filter();
  client_filter->set_all(true);

  ListClients(
      request.get(), response.get(),
      base::BindOnce(&OnListAllClients, event_, client_id_, response.get()));

  response->clear_client_infos();
  uint32_t id;
  {
    DECLARE_REQUEST_AND_RESPONSE(RegisterClient);
    RegisterRandomClientForTesting(request.get(), response.get());
    id = response->id();
  }
  client_filter->Clear();
  client_filter->set_id(id);

  ListClients(request.get(), response.get(),
              base::BindOnce(&OnListClient, event_, id, response.get()));
}

TEST_F(MasterTest, RegisterNode) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterNode);

  NodeInfo* node_info = request->mutable_node_info();

  RegisterNode(request.get(), response.get(),
               base::BindOnce(&ExpectClientNotRegistered, event_));

  node_info->set_client_id(client_id_);
  node_info->set_name(publishing_node_name_);

  RegisterNode(
      request.get(), response.get(),
      base::BindOnce(&ExpectNodeAlreadyRegistered, event_, *node_info));

  node_info->clear_name();

  RegisterNode(request.get(), response.get(),
               base::BindOnce(&ExpectOK, event_));
}

#define EXPECT_CHECK_NODE_EXISTS(Method, node_info)           \
  Method(request.get(), response.get(),                       \
         base::BindOnce(&ExpectClientNotRegistered, event_)); \
  node_info->set_client_id(client_id_);                       \
  Method(request.get(), response.get(),                       \
         base::BindOnce(&ExpectNodeNotRegistered, event_, *node_info))

namespace {

void OnListAllNodes(std::shared_ptr<base::WaitableEvent> event,
                    uint32_t client_id, const std::vector<std::string>& names,
                    ListNodesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(4, node_infos.size());
  for (int i = 0; i < node_infos.size(); ++i) {
    EXPECT_EQ(names[i], node_infos[i].name());
    EXPECT_EQ(client_id, node_infos[i].client_id());
  }
  event->Signal();
}

void OnListPublishingNodes(std::shared_ptr<base::WaitableEvent> event,
                           uint32_t client_id,
                           const std::string& publishing_node_name,
                           ListNodesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(1, node_infos.size());
  EXPECT_EQ(publishing_node_name, node_infos[0].name());
  EXPECT_EQ(client_id, node_infos[0].client_id());
  event->Signal();
}

void OnListSubscribingNodes(std::shared_ptr<base::WaitableEvent> event,
                            uint32_t client_id,
                            const std::string& subscribing_node_name,
                            ListNodesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(1, node_infos.size());
  EXPECT_EQ(subscribing_node_name, node_infos[0].name());
  EXPECT_EQ(client_id, node_infos[0].client_id());
  event->Signal();
}

void OnListPubSubTopics(std::shared_ptr<base::WaitableEvent> event,
                        const std::string& topic, ListNodesResponse* response,
                        const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& pub_sub_topics = response->pub_sub_topics();
  EXPECT_EQ(1, pub_sub_topics.publishing_topics().size());
  EXPECT_EQ(topic, pub_sub_topics.publishing_topics()[0]);
  EXPECT_EQ(0, pub_sub_topics.subscribing_topics().size());
  event->Signal();
}

}  // namespace

TEST_F(MasterTest, ListNodes) {
  DECLARE_REQUEST_AND_RESPONSE(ListNodes);

  NodeFilter* node_filter = request->mutable_node_filter();
  node_filter->set_all(true);

  ListNodes(request.get(), response.get(),
            base::BindOnce(&OnListAllNodes, event_, client_id_,
                           std::vector<std::string>{
                               publishing_node_name_, subscribing_node_name_,
                               client_node_name_, server_node_name_},
                           response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_publishing_topic(topic_);

  ListNodes(request.get(), response.get(),
            base::BindOnce(&OnListPublishingNodes, event_, client_id_,
                           publishing_node_name_, response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_subscribing_topic(topic_);

  ListNodes(request.get(), response.get(),
            base::BindOnce(&OnListSubscribingNodes, event_, client_id_,
                           subscribing_node_name_, response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_name(publishing_node_name_);

  ListNodes(
      request.get(), response.get(),
      base::BindOnce(&OnListPubSubTopics, event_, topic_, response.get()));
}

TEST_F(MasterTest, PublishTopic) {
  DECLARE_REQUEST_AND_RESPONSE(PublishTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(PublishTopic, node_info);

  node_info->set_name(publishing_node_name_);
  TopicInfo* topic_info = request->mutable_topic_info();

  PublishTopic(request.get(), response.get(),
               base::BindOnce(&ExpectChannelSourceNotValid, event_,
                              "topic source", topic_info->topic_source()));

  PreparePublishTopic(request.get(), response.get(), *node_info, topic_,
                      topic_info);
  PublishTopic(request.get(), response.get(),
               base::BindOnce(&ExpectTopicAlreadyPublishingOnNode, event_,
                              *node_info, *topic_info));

  topic_info->set_topic("topic2");

  PublishTopic(request.get(), response.get(),
               base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, UnpublishTopic) {
  DECLARE_REQUEST_AND_RESPONSE(UnpublishTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnpublishTopic, node_info);

  node_info->set_name(publishing_node_name_);
  request->set_topic("topic2");

  UnpublishTopic(request.get(), response.get(),
                 base::BindOnce(&ExpectTopicNotPublishingOnNode, event_,
                                *node_info, request->topic()));

  request->set_topic(topic_);

  UnpublishTopic(request.get(), response.get(),
                 base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, SubscribeTopic) {
  DECLARE_REQUEST_AND_RESPONSE(SubscribeTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(SubscribeTopic, node_info);

  node_info->set_name(subscribing_node_name_);
  request->set_topic(topic_);

  SubscribeTopic(request.get(), response.get(),
                 base::BindOnce(&ExpectTopicAlreadySubscribingOnNode, event_,
                                *node_info, request->topic()));

  request->set_topic("topic2");

  SubscribeTopic(request.get(), response.get(),
                 base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, UnsubscribeTopic) {
  DECLARE_REQUEST_AND_RESPONSE(UnsubscribeTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnsubscribeTopic, node_info);

  node_info->set_name(subscribing_node_name_);
  request->set_topic("topic2");

  UnsubscribeTopic(request.get(), response.get(),
                   base::BindOnce(&ExpectTopicNotSubscribingOnNode, event_,
                                  *node_info, request->topic()));

  request->set_topic(topic_);

  UnsubscribeTopic(request.get(), response.get(),
                   base::BindOnce(&ExpectOK, event_));
}

void OnListAllTopics(std::shared_ptr<base::WaitableEvent> event,
                     const std::string& topic,
                     const ChannelSource& topic_source,
                     ListTopicsResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& topic_infos = response->topic_infos();
  EXPECT_EQ(1, topic_infos.size());
  EXPECT_EQ(topic, topic_infos[0].topic());
  EXPECT_TRUE(IsSameChannelSource(topic_source, topic_infos[0].topic_source()));
  event->Signal();
}

void OnListTopic(std::shared_ptr<base::WaitableEvent> event,
                 ListTopicsResponse* response, const TopicInfo& topic_info,
                 const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& topic_infos = response->topic_infos();
  EXPECT_EQ(1, topic_infos.size());
  EXPECT_EQ(topic_info.topic(), topic_infos[0].topic());
  EXPECT_TRUE(IsSameChannelSource(topic_info.topic_source(),
                                  topic_infos[0].topic_source()));
  event->Signal();
}

TEST_F(MasterTest, ListTopics) {
  DECLARE_REQUEST_AND_RESPONSE(ListTopics);

  TopicFilter* topic_filter = request->mutable_topic_filter();
  topic_filter->set_all(true);

  ListTopics(request.get(), response.get(),
             base::BindOnce(&OnListAllTopics, event_, topic_,
                            topic_info_.topic_source(), response.get()));

  response->clear_topic_infos();
  std::string topic = "test2";
  TopicInfo topic_info;
  {
    DECLARE_REQUEST_AND_RESPONSE(PublishTopic);
    PreparePublishTopic(request.get(), response.get(), pub_node_info_, topic,
                        &topic_info);
    PublishTopic(request.get(), response.get(),
                 base::BindOnce(&ExpectOK, event_));
  }
  topic_filter->Clear();
  topic_filter->set_topic(topic);

  ListTopics(request.get(), response.get(),
             base::BindOnce(&OnListTopic, event_, response.get(), topic_info));
}

TEST_F(MasterTest, RegisterServiceClient) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterServiceClient);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(RegisterServiceClient, node_info);

  node_info->set_name(client_node_name_);
  request->set_service(service_);

  RegisterServiceClient(request.get(), response.get(),
                        base::BindOnce(&ExpectServiceAlreadyRequestingOnNode,
                                       event_, *node_info, request->service()));

  request->set_service("service2");

  RegisterServiceClient(request.get(), response.get(),
                        base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, UnregisterServiceClient) {
  DECLARE_REQUEST_AND_RESPONSE(UnregisterServiceClient);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnregisterServiceClient, node_info);

  node_info->set_name(client_node_name_);
  request->set_service("service2");

  UnregisterServiceClient(
      request.get(), response.get(),
      base::BindOnce(&ExpectServiceNotRequestingOnNode, event_, *node_info,
                     request->service()));

  request->set_service(service_);

  UnregisterServiceClient(request.get(), response.get(),
                          base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, RegisterServiceServer) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterServiceServer);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(RegisterServiceServer, node_info);

  node_info->set_name(server_node_name_);
  ServiceInfo* service_info = request->mutable_service_info();

  RegisterServiceServer(
      request.get(), response.get(),
      base::BindOnce(&ExpectChannelSourceNotValid, event_, "service source",
                     service_info->service_source()));

  PrepareRegisterServiceServer(request.get(), response.get(), *node_info,
                               service_, service_info);
  RegisterServiceServer(request.get(), response.get(),
                        base::BindOnce(&ExpectServiceAlreadyServingOnNode,
                                       event_, *node_info, *service_info));

  service_info->set_service("service2");

  RegisterServiceServer(request.get(), response.get(),
                        base::BindOnce(&ExpectOK, event_));
}

TEST_F(MasterTest, UnregisterServiceServer) {
  DECLARE_REQUEST_AND_RESPONSE(UnregisterServiceServer);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnregisterServiceServer, node_info);

  node_info->set_name(server_node_name_);
  request->set_service("service2");

  UnregisterServiceServer(request.get(), response.get(),
                          base::BindOnce(&ExpectServiceNotServingOnNode, event_,
                                         *node_info, request->service()));

  request->set_service(service_);

  UnregisterServiceServer(request.get(), response.get(),
                          base::BindOnce(&ExpectOK, event_));
}

void OnListAllServices(std::shared_ptr<base::WaitableEvent> event,
                       const std::string& service,
                       const ChannelSource& service_source,
                       ListServicesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& service_infos = response->service_infos();
  EXPECT_EQ(1, service_infos.size());
  EXPECT_EQ(service, service_infos[0].service());
  EXPECT_TRUE(
      IsSameChannelSource(service_source, service_infos[0].service_source()));
  event->Signal();
}

void OnListService(std::shared_ptr<base::WaitableEvent> event,
                   ListServicesResponse* response,
                   const ServiceInfo& service_info, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& service_infos = response->service_infos();
  EXPECT_EQ(1, service_infos.size());
  EXPECT_EQ(service_info.service(), service_infos[0].service());
  EXPECT_TRUE(IsSameChannelSource(service_info.service_source(),
                                  service_infos[0].service_source()));
  event->Signal();
}

TEST_F(MasterTest, ListServices) {
  DECLARE_REQUEST_AND_RESPONSE(ListServices);

  ServiceFilter* service_filter = request->mutable_service_filter();
  service_filter->set_all(true);

  ListServices(request.get(), response.get(),
               base::BindOnce(&OnListAllServices, event_, service_,
                              service_info_.service_source(), response.get()));

  response->clear_service_infos();
  std::string service = "test2";
  ServiceInfo service_info;
  {
    DECLARE_REQUEST_AND_RESPONSE(RegisterServiceServer);
    PrepareRegisterServiceServer(request.get(), response.get(), pub_node_info_,
                                 service, &service_info);
    RegisterServiceServer(request.get(), response.get(),
                          base::BindOnce(&ExpectOK, event_));
  }
  service_filter->Clear();
  service_filter->set_service(service);

  ListServices(
      request.get(), response.get(),
      base::BindOnce(&OnListService, event_, response.get(), service_info));
}

#undef EXPECT_CHECK_NODE_EXISTS
#undef DECLARE_REQUEST_AND_RESPONSE

}  // namespace felicia