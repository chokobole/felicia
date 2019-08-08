#include "felicia/core/master/master.h"

#include <memory>

#include "gtest/gtest.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/lib/strings/str_util.h"
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
  event->Signal();
}

void ExpectChannelSourceNotValid(std::shared_ptr<base::WaitableEvent> event,
                                 const std::string& name,
                                 const ChannelSource& channel_source,
                                 const Status& s) {
  Status expected = errors::ChannelSourceNotValid(name, channel_source);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectClientNotRegistered(std::shared_ptr<base::WaitableEvent> event,
                               const Status& s) {
  Status expected = errors::ClientNotRegistered();
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectNodeNotRegistered(std::shared_ptr<base::WaitableEvent> event,
                             const NodeInfo& node_info, const Status& s) {
  Status expected = errors::NodeNotRegistered(node_info);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectNodeAlreadyRegistered(std::shared_ptr<base::WaitableEvent> event,
                                 const NodeInfo& node_info, const Status& s) {
  Status expected = errors::NodeAlreadyRegistered(node_info);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectTopicAlreadyPublishing(std::shared_ptr<base::WaitableEvent> event,
                                  const TopicInfo& topic_info,
                                  const Status& s) {
  Status expected = errors::TopicAlreadyPublishing(topic_info);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectTopicNotPublishingOnNode(std::shared_ptr<base::WaitableEvent> event,
                                    const NodeInfo& node_info,
                                    const std::string& topic, const Status& s) {
  Status expected = errors::TopicNotPublishingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectTopicAlreadySubscribingOnNode(
    std::shared_ptr<base::WaitableEvent> event, const NodeInfo& node_info,
    const std::string& topic, const Status& s) {
  Status expected = errors::TopicAlreadySubscribingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

void ExpectTopicNotSubscribingOnNode(std::shared_ptr<base::WaitableEvent> event,
                                     const NodeInfo& node_info,
                                     const std::string& topic,
                                     const Status& s) {
  Status expected = errors::TopicNotSubscribingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
  event->Signal();
}

}  // namespace

#define DECLARE_REQUEST_AND_RESPONSE(Method)          \
  auto request = std::make_unique<Method##Request>(); \
  auto response = std::make_unique<Method##Response>()

class MasterTest : public ::testing::Test {
 public:
  MasterTest()
      : master_(base::WrapUnique(new Master())),
        publishing_node_name_("publisher"),
        subscribing_node_name_("subscriber"),
        topic_("topic"),
        event_(std::make_shared<base::WaitableEvent>(
            base::WaitableEvent::ResetPolicy::AUTOMATIC,
            base::WaitableEvent::InitialState::NOT_SIGNALED)) {
    master_->SetCheckHeartBeatForTesting(false);
    master_->Run();
  }

  void SetUp() override {
    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterClient);
      RegisterRandomClientForTesting(request.get(), response.get());
      client_id_ = response->id();
    }

    pub_node_info_.set_client_id(client_id_);
    pub_node_info_.set_name(publishing_node_name_);
    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterNode);
      *request->mutable_node_info() = pub_node_info_;
      RegisterNode(request.get(), response.get(),
                   base::BindOnce(&ExpectOK, event_));
    }

    {
      DECLARE_REQUEST_AND_RESPONSE(PublishTopic);
      PublishTopicForTesting(request.get(), response.get(), pub_node_info_,
                             topic_, &topic_info_);
    }

    sub_node_info_.set_client_id(client_id_);
    sub_node_info_.set_name(subscribing_node_name_);
    {
      DECLARE_REQUEST_AND_RESPONSE(RegisterNode);
      *request->mutable_node_info() = sub_node_info_;
      RegisterNode(request.get(), response.get(),
                   base::BindOnce(&ExpectOK, event_));
    }

    {
      DECLARE_REQUEST_AND_RESPONSE(SubscribeTopic);
      *request->mutable_node_info() = sub_node_info_;
      *request->mutable_topic() = topic_;
      SubscribeTopic(request.get(), response.get(),
                     base::BindOnce(&ExpectOK, event_));
    }
  }

  void TearDown() override {
    {
      DECLARE_REQUEST_AND_RESPONSE(UnregisterNode);
      *request->mutable_node_info() = pub_node_info_;
      UnregisterNode(request.get(), response.get(),
                     base::BindOnce(&ExpectOK, event_));
    }

    {
      DECLARE_REQUEST_AND_RESPONSE(UnregisterNode);
      *request->mutable_node_info() = sub_node_info_;
      UnregisterNode(request.get(), response.get(),
                     base::BindOnce(&ExpectOK, event_));
    }
  }

#define MASTER_METHOD(method)                                       \
  void method(const method##Request* arg, method##Response* result, \
              StatusOnceCallback callback) {                        \
    master_->method(arg, result, std::move(callback));              \
    event_->Wait();                                                 \
  }

  MASTER_METHOD(RegisterClient)
  MASTER_METHOD(ListClients)
  MASTER_METHOD(RegisterNode)
  MASTER_METHOD(UnregisterNode)
  MASTER_METHOD(ListNodes)
  MASTER_METHOD(PublishTopic)
  MASTER_METHOD(UnpublishTopic)
  MASTER_METHOD(SubscribeTopic)
  MASTER_METHOD(UnsubscribeTopic)
  MASTER_METHOD(ListTopics)

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
    ChannelSource topic_info_watcher_source;
    *topic_info_watcher_source.add_channel_defs() = channel_def;
    *client_info.mutable_topic_info_watcher_source() =
        topic_info_watcher_source;
    *request->mutable_client_info() = client_info;
    RegisterClient(request, response, base::BindOnce(&ExpectOK, event_));
  }

  void PublishTopicForTesting(PublishTopicRequest* request,
                              PublishTopicResponse* response,
                              const NodeInfo& node_info,
                              const std::string& topic, TopicInfo* topic_info) {
    topic_info->set_topic(topic);
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    FillRandomTCPChannelDef(&channel_def);
    ChannelSource* topic_source = topic_info->mutable_topic_source();
    *topic_source->add_channel_defs() = channel_def;
    *request->mutable_node_info() = node_info;
    *request->mutable_topic_info() = *topic_info;
    PublishTopic(request, response, base::BindOnce(&ExpectOK, event_));
  }

 protected:
  std::unique_ptr<Master> master_;
  uint32_t client_id_;
  std::string publishing_node_name_;
  std::string subscribing_node_name_;
  std::string topic_;
  TopicInfo topic_info_;
  NodeInfo pub_node_info_;
  NodeInfo sub_node_info_;
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
                    uint32_t client_id, const std::string& publishing_node_name,
                    const std::string& subscribing_node_name,
                    ListNodesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(2, node_infos.size());
  EXPECT_TRUE(strings::Equals(publishing_node_name, node_infos[0].name()));
  EXPECT_EQ(client_id, node_infos[0].client_id());
  EXPECT_TRUE(strings::Equals(subscribing_node_name, node_infos[1].name()));
  EXPECT_EQ(client_id, node_infos[1].client_id());
  event->Signal();
}

void OnListPublishingNodes(std::shared_ptr<base::WaitableEvent> event,
                           uint32_t client_id,
                           const std::string& publishing_node_name,
                           ListNodesResponse* response, const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(1, node_infos.size());
  EXPECT_TRUE(strings::Equals(publishing_node_name, node_infos[0].name()));
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
  EXPECT_TRUE(strings::Equals(subscribing_node_name, node_infos[0].name()));
  EXPECT_EQ(client_id, node_infos[0].client_id());
  event->Signal();
}

void OnListPubSubTopics(std::shared_ptr<base::WaitableEvent> event,
                        const std::string& topic, ListNodesResponse* response,
                        const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& pub_sub_topics = response->pub_sub_topics();
  EXPECT_EQ(1, pub_sub_topics.publishing_topics().size());
  EXPECT_TRUE(strings::Equals(topic, pub_sub_topics.publishing_topics()[0]));
  EXPECT_EQ(0, pub_sub_topics.subscribing_topics().size());
  event->Signal();
}

}  // namespace

TEST_F(MasterTest, ListNodes) {
  DECLARE_REQUEST_AND_RESPONSE(ListNodes);

  NodeFilter* node_filter = request->mutable_node_filter();
  node_filter->set_all(true);

  ListNodes(
      request.get(), response.get(),
      base::BindOnce(&OnListAllNodes, event_, client_id_, publishing_node_name_,
                     subscribing_node_name_, response.get()));

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

  topic_info->set_topic(topic_);
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
  FillRandomTCPChannelDef(&channel_def);
  *topic_info->mutable_topic_source()->add_channel_defs() = channel_def;

  PublishTopic(
      request.get(), response.get(),
      base::BindOnce(&ExpectTopicAlreadyPublishing, event_, *topic_info));

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
  EXPECT_TRUE(strings::Equals(topic, topic_infos[0].topic()));
  EXPECT_TRUE(IsSameChannelSource(topic_source, topic_infos[0].topic_source()));
  event->Signal();
}

void OnListTopic(std::shared_ptr<base::WaitableEvent> event,
                 ListTopicsResponse* response, const TopicInfo& topic_info,
                 const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& topic_infos = response->topic_infos();
  EXPECT_EQ(1, topic_infos.size());
  EXPECT_TRUE(strings::Equals(topic_info.topic(), topic_infos[0].topic()));
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
    PublishTopicForTesting(request.get(), response.get(), pub_node_info_, topic,
                           &topic_info);
  }
  topic_filter->Clear();
  topic_filter->set_topic(topic);

  ListTopics(request.get(), response.get(),
             base::BindOnce(&OnListTopic, event_, response.get(), topic_info));
}

#undef EXPECT_CHECK_NODE_EXISTS
#undef DECLARE_REQUEST_AND_RESPONSE

}  // namespace felicia