#include "felicia/core/master/master.h"

#include <memory>

#include "gtest/gtest.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/errors.h"

namespace felicia {

std::unique_ptr<Master> NewMasterForTesting() {
  return ::base::WrapUnique(new Master);
}

namespace {

void ExpectOK(const Status& s) { EXPECT_TRUE(s.ok()); }

void ExpectChannelSourceNotValid(const std::string& name,
                                 const ChannelSource& channel_source,
                                 const Status& s) {
  Status expected = errors::ChannelSourceNotValid(name, channel_source);
  EXPECT_TRUE(s == expected);
}

void ExpectClientNotRegistered(const Status& s) {
  Status expected = errors::ClientNotRegistered();
  EXPECT_TRUE(s == expected);
}

void ExpectNodeNotRegistered(const NodeInfo& node_info, const Status& s) {
  Status expected = errors::NodeNotRegistered(node_info);
  EXPECT_TRUE(s == expected);
}

void ExpectNodeAlreadyRegistered(const NodeInfo& node_info, const Status& s) {
  Status expected = errors::NodeAlreadyRegistered(node_info);
  EXPECT_TRUE(s == expected);
}

void ExpectTopicAlreadyPublishing(const TopicInfo& topic_info,
                                  const Status& s) {
  Status expected = errors::TopicAlreadyPublishing(topic_info);
  EXPECT_TRUE(s == expected);
}

void ExpectTopicNotPublishingOnNode(const NodeInfo& node_info,
                                    const std::string& topic, const Status& s) {
  Status expected = errors::TopicNotPublishingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
}

void ExpectTopicAlreadySubscribingOnNode(const NodeInfo& node_info,
                                         const std::string& topic,
                                         const Status& s) {
  Status expected = errors::TopicAlreadySubscribingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
}

void ExpectTopicNotSubscribingOnNode(const NodeInfo& node_info,
                                     const std::string& topic,
                                     const Status& s) {
  Status expected = errors::TopicNotSubscribingOnNode(node_info, topic);
  EXPECT_TRUE(s == expected);
}

}  // namespace

class MockMaster : public Master {};

class MasterTest : public ::testing::Test {
 public:
  MasterTest()
      : master_(NewMasterForTesting()),
        publishing_node_name_("publisher"),
        subscribing_node_name_("subscriber"),
        topic_("topic"),
        topic_source_(PickRandomChannelSource(ChannelDef_Type_TCP)) {}

  void SetUp() override {
    ClientInfo client_info;
    std::unique_ptr<Client> client = Client::NewClient(client_info);
    client_id_ = client->client_info().id();
    master_->AddClient(client_id_, std::move(client));

    NodeInfo pub_node_info;
    pub_node_info.set_client_id(client_id_);
    pub_node_info.set_name(publishing_node_name_);
    std::unique_ptr<Node> pub_node = Node::NewNode(pub_node_info);

    TopicInfo topic_info;
    topic_info.set_topic(topic_);
    *topic_info.mutable_topic_source() = topic_source_;
    pub_node->RegisterPublishingTopic(topic_info);
    master_->AddNode(std::move(pub_node));

    NodeInfo sub_node_info;
    sub_node_info.set_client_id(client_id_);
    sub_node_info.set_name(subscribing_node_name_);
    std::unique_ptr<Node> sub_node = Node::NewNode(sub_node_info);

    sub_node->RegisterSubscribingTopic(topic_);
    master_->AddNode(std::move(sub_node));
  }

  void TearDown() override {
    ClientInfo client_info;
    client_info.set_id(client_id_);
    master_->RemoveClient(client_info);
  }

  void PublishTopicForTesting(const TopicInfo& topic_info) {
    NodeInfo node_info;
    node_info.set_client_id(client_id_);
    node_info.set_name(publishing_node_name_);
    ::base::WeakPtr<Node> node = master_->FindNode(node_info);
    node->RegisterPublishingTopic(topic_info);
  }

 protected:
  friend void OnListAllNodes(MasterTest*, ListNodesResponse*, const Status&);
  friend void OnListPublishingNodes(MasterTest*, ListNodesResponse*,
                                    const Status&);
  friend void OnListSubscribingNodes(MasterTest*, ListNodesResponse*,
                                     const Status&);
  friend void OnListPubSubTopics(MasterTest*, ListNodesResponse*,
                                 const Status&);
  friend void OnListAllTopics(MasterTest*, ListTopicsResponse*, const Status&);
  friend void OnListTopic(ListTopicsResponse*, const TopicInfo&, const Status&);

  std::unique_ptr<Master> master_;
  uint32_t client_id_;
  std::string publishing_node_name_;
  std::string subscribing_node_name_;
  std::string topic_;
  ChannelSource topic_source_;
};

#define DECLARE_REQUEST_AND_RESPONSE(Method)          \
  auto request = std::make_unique<Method##Request>(); \
  auto response = std::make_unique<Method##Response>()

TEST_F(MasterTest, RegisterClient) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterClient);

  ClientInfo* client_info = request->mutable_client_info();

  master_->RegisterClient(
      request.get(), response.get(),
      ::base::BindOnce(&ExpectChannelSourceNotValid, "heart beat signaller",
                       client_info->heart_beat_signaller_source()));
}

TEST_F(MasterTest, RegisterNode) {
  DECLARE_REQUEST_AND_RESPONSE(RegisterNode);

  NodeInfo* node_info = request->mutable_node_info();

  master_->RegisterNode(request.get(), response.get(),
                        ::base::BindOnce(&ExpectClientNotRegistered));

  node_info->set_client_id(client_id_);
  node_info->set_name(publishing_node_name_);

  master_->RegisterNode(
      request.get(), response.get(),
      ::base::BindOnce(&ExpectNodeAlreadyRegistered, *node_info));

  node_info->clear_name();

  master_->RegisterNode(request.get(), response.get(),
                        ::base::BindOnce(&ExpectOK));
}

#define EXPECT_CHECK_NODE_EXISTS(Method, node_info)              \
  master_->Method(request.get(), response.get(),                 \
                  ::base::BindOnce(&ExpectClientNotRegistered)); \
  node_info->set_client_id(client_id_);                          \
  master_->Method(request.get(), response.get(),                 \
                  ::base::BindOnce(&ExpectNodeNotRegistered, *node_info))

TEST_F(MasterTest, UnregisterNode) {
  DECLARE_REQUEST_AND_RESPONSE(UnregisterNode);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnregisterNode, node_info);

  node_info->set_name(publishing_node_name_);

  master_->UnregisterNode(request.get(), response.get(),
                          ::base::BindOnce(&ExpectOK));
}

void OnListAllNodes(MasterTest* test, ListNodesResponse* response,
                    const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(2, node_infos.size());
  EXPECT_TRUE(
      strings::Equals(test->publishing_node_name_, node_infos[0].name()));
  EXPECT_EQ(test->client_id_, node_infos[0].client_id());
  EXPECT_TRUE(
      strings::Equals(test->subscribing_node_name_, node_infos[1].name()));
  EXPECT_EQ(test->client_id_, node_infos[1].client_id());
}

void OnListPublishingNodes(MasterTest* test, ListNodesResponse* response,
                           const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(1, node_infos.size());
  EXPECT_TRUE(
      strings::Equals(test->publishing_node_name_, node_infos[0].name()));
  EXPECT_EQ(test->client_id_, node_infos[0].client_id());
}

void OnListSubscribingNodes(MasterTest* test, ListNodesResponse* response,
                            const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& node_infos = response->node_infos();
  EXPECT_EQ(1, node_infos.size());
  EXPECT_TRUE(
      strings::Equals(test->subscribing_node_name_, node_infos[0].name()));
  EXPECT_EQ(test->client_id_, node_infos[0].client_id());
}

void OnListPubSubTopics(MasterTest* test, ListNodesResponse* response,
                        const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& pub_sub_topics = response->pub_sub_topics();
  EXPECT_EQ(1, pub_sub_topics.publishing_topics().size());
  EXPECT_TRUE(
      strings::Equals(test->topic_, pub_sub_topics.publishing_topics()[0]));
  EXPECT_EQ(0, pub_sub_topics.subscribing_topics().size());
}

TEST_F(MasterTest, ListNodes) {
  DECLARE_REQUEST_AND_RESPONSE(ListNodes);

  NodeFilter* node_filter = request->mutable_node_filter();
  node_filter->set_all(true);

  master_->ListNodes(request.get(), response.get(),
                     ::base::BindOnce(&OnListAllNodes, this, response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_publishing_topic(topic_);

  master_->ListNodes(
      request.get(), response.get(),
      ::base::BindOnce(&OnListPublishingNodes, this, response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_subscribing_topic(topic_);

  master_->ListNodes(
      request.get(), response.get(),
      ::base::BindOnce(&OnListSubscribingNodes, this, response.get()));

  response->clear_node_infos();
  node_filter->Clear();
  node_filter->set_name(publishing_node_name_);

  master_->ListNodes(
      request.get(), response.get(),
      ::base::BindOnce(&OnListPubSubTopics, this, response.get()));
}

TEST_F(MasterTest, PublishTopic) {
  DECLARE_REQUEST_AND_RESPONSE(PublishTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(PublishTopic, node_info);

  node_info->set_name(publishing_node_name_);
  TopicInfo* topic_info = request->mutable_topic_info();

  master_->PublishTopic(
      request.get(), response.get(),
      ::base::BindOnce(&ExpectChannelSourceNotValid, "topic source",
                       topic_info->topic_source()));

  topic_info->set_topic(topic_);
  *topic_info->mutable_topic_source() =
      PickRandomChannelSource(ChannelDef_Type_TCP);

  master_->PublishTopic(
      request.get(), response.get(),
      ::base::BindOnce(&ExpectTopicAlreadyPublishing, *topic_info));

  topic_info->set_topic("topic2");

  EXPECT_DEATH_IF_SUPPORTED(master_->PublishTopic(request.get(), response.get(),
                                                  ::base::BindOnce(&ExpectOK)),
                            "");
}

TEST_F(MasterTest, UnpublishTopic) {
  DECLARE_REQUEST_AND_RESPONSE(UnpublishTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnpublishTopic, node_info);

  node_info->set_name(publishing_node_name_);
  request->set_topic("topic2");

  master_->UnpublishTopic(request.get(), response.get(),
                          ::base::BindOnce(&ExpectTopicNotPublishingOnNode,
                                           *node_info, request->topic()));

  request->set_topic(topic_);

  master_->UnpublishTopic(request.get(), response.get(),
                          ::base::BindOnce(&ExpectOK));
}

TEST_F(MasterTest, SubscribeTopic) {
  DECLARE_REQUEST_AND_RESPONSE(SubscribeTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(SubscribeTopic, node_info);

  node_info->set_name(subscribing_node_name_);
  request->set_topic(topic_);

  master_->SubscribeTopic(request.get(), response.get(),
                          ::base::BindOnce(&ExpectTopicAlreadySubscribingOnNode,
                                           *node_info, request->topic()));

  request->set_topic("topic2");

  EXPECT_DEATH_IF_SUPPORTED(
      master_->SubscribeTopic(request.get(), response.get(),
                              ::base::BindOnce(&ExpectOK)),
      "");
}

TEST_F(MasterTest, UnsubscribeTopic) {
  DECLARE_REQUEST_AND_RESPONSE(UnsubscribeTopic);

  NodeInfo* node_info = request->mutable_node_info();

  EXPECT_CHECK_NODE_EXISTS(UnsubscribeTopic, node_info);

  node_info->set_name(subscribing_node_name_);
  request->set_topic("topic2");

  master_->UnsubscribeTopic(request.get(), response.get(),
                            ::base::BindOnce(&ExpectTopicNotSubscribingOnNode,
                                             *node_info, request->topic()));

  request->set_topic(topic_);

  master_->UnsubscribeTopic(request.get(), response.get(),
                            ::base::BindOnce(&ExpectOK));
}

void OnListAllTopics(MasterTest* test, ListTopicsResponse* response,
                     const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& topic_infos = response->topic_infos();
  EXPECT_EQ(1, topic_infos.size());
  EXPECT_TRUE(strings::Equals(test->topic_, topic_infos[0].topic()));
  EXPECT_TRUE(
      IsSameChannelSource(test->topic_source_, topic_infos[0].topic_source()));
}

void OnListTopic(ListTopicsResponse* response, const TopicInfo& topic_info,
                 const Status& s) {
  EXPECT_TRUE(s.ok());
  auto& topic_infos = response->topic_infos();
  EXPECT_EQ(1, topic_infos.size());
  EXPECT_TRUE(strings::Equals(topic_info.topic(), topic_infos[0].topic()));
  EXPECT_TRUE(IsSameChannelSource(topic_info.topic_source(),
                                  topic_infos[0].topic_source()));
}

TEST_F(MasterTest, ListTopics) {
  DECLARE_REQUEST_AND_RESPONSE(ListTopics);

  TopicFilter* topic_filter = request->mutable_topic_filter();
  topic_filter->set_all(true);

  master_->ListTopics(request.get(), response.get(),
                      ::base::BindOnce(&OnListAllTopics, this, response.get()));

  response->clear_topic_infos();
  std::string topic = "test2";
  TopicInfo topic_info;
  topic_info.set_topic(topic);
  *topic_info.mutable_topic_source() =
      PickRandomChannelSource(ChannelDef_Type_TCP);
  PublishTopicForTesting(topic_info);
  topic_filter->Clear();
  topic_filter->set_topic(topic);

  master_->ListTopics(
      request.get(), response.get(),
      ::base::BindOnce(&OnListTopic, response.get(), topic_info));
}

#undef EXPECT_CHECK_NODE_EXISTS
#undef DECLARE_REQUEST_AND_RESPONSE

}  // namespace felicia