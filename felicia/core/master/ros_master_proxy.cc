#if defined(HAS_ROS)

#include <algorithm>

#include <ros/network.h>
#include <ros/xmlrpc_manager.h>

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/ros_master_proxy.h"

namespace felicia {

namespace {

Status FailedToExecute(const std::string& method) {
  return errors::Unavailable(
      base::StringPrintf("Failed to execute %s.", method.c_str()));
}

Status Execute(const std::string& method, const XmlRpc::XmlRpcValue& request,
               XmlRpc::XmlRpcValue& response, XmlRpc::XmlRpcValue& payload,
               bool wait_for_master) {
  if (ros::master::execute(method, request, response, payload, wait_for_master))
    return Status::OK();
  return FailedToExecute(method);
}

Status Execute(XmlRpc::XmlRpcClient* client, const std::string& method,
               const XmlRpc::XmlRpcValue& request,
               XmlRpc::XmlRpcValue& response, XmlRpc::XmlRpcValue& payload) {
  if (!client->execute(method.c_str(), request, response))
    return FailedToExecute(method);
  XmlRpc::XmlRpcValue proto;
  if (!ros::XMLRPCManager::instance()->validateXmlrpcResponse(
          method.c_str(), response, payload))
    return errors::InvalidArgument(
        base::StringPrintf("Invalid response for method: %s.", method.c_str()));
  return Status::OK();
}

}  // namespace

ROSMasterProxy::TopicType::TopicType() = default;

ROSMasterProxy::TopicType::TopicType(const std::string& topic,
                                     const std::string& topic_type)
    : topic(topic), topic_type(topic_type) {}

ROSMasterProxy::ROSMasterProxy() = default;

ROSMasterProxy::~ROSMasterProxy() = default;

// static
ROSMasterProxy& ROSMasterProxy::GetInstance() {
  static base::NoDestructor<ROSMasterProxy> ros_master_proxy;
  return *ros_master_proxy;
}

Status ROSMasterProxy::Init(Master* master) {
  master_ = master;
  ros::M_string remappings;
  ros::init(remappings, "ros_master_proxy", ros::init_options::NoSigintHandler);
  xmlrpc_manager_ = ros::XMLRPCManager::instance();
  xmlrpc_manager_->bind(
      "publisherUpdate",
      boost::bind(&ROSMasterProxy::PubUpdateCallback, this, _1, _2));

  return Status::OK();
}

Status ROSMasterProxy::Start() {
  xmlrpc_manager_->start();
  return Status::OK();
}

Status ROSMasterProxy::Shutdown() {
  xmlrpc_manager_->shutdown();
  return Status::OK();
}

Status ROSMasterProxy::GetPid() const {
  XmlRpc::XmlRpcValue request, response, payload;
  request[0] = ros::this_node::getName();
  return Execute("getPid", request, response, payload, false);
}

Status ROSMasterProxy::GetPublishedTopicTypes(
    std::vector<TopicType>* topic_types) const {
  XmlRpc::XmlRpcValue request, response, payload;
  request[0] = ros::this_node::getName();
  request[1] = "";
  Status s = Execute("getPublishedTopics", request, response, payload, false);
  if (!s.ok()) return s;
  topic_types->clear();
  for (int i = 0; i < payload.size(); ++i) {
    topic_types->emplace_back(std::string(payload[i][0]),
                              std::string(payload[i][1]));
  }
  return Status::OK();
}

Status ROSMasterProxy::GetPublishedTopicType(const std::string& topic,
                                             std::string* topic_type) const {
  std::vector<TopicType> topic_types;
  Status s = GetPublishedTopicTypes(&topic_types);
  if (!s.ok()) return s;
  auto it = std::find_if(topic_types.begin(), topic_types.end(),
                         [&topic](const TopicType& topic_type) {
                           return topic_type.topic == topic;
                         });
  if (it != topic_types.end()) {
    *topic_type = it->topic_type;
    return Status::OK();
  }
  return errors::NotFound(base::StringPrintf(
      "Failed to find type for published topic: %s.", topic.c_str()));
}

Status ROSMasterProxy::RegisterSubscriber(const std::string& topic,
                                          const std::string& topic_type) {
  XmlRpc::XmlRpcValue request, response, payload;
  const std::string& this_server_uri = xmlrpc_manager_->getServerURI();
  request[0] = ros::this_node::getName();
  request[1] = topic;
  request[2] = topic_type;
  request[3] = this_server_uri;
  Status s = Execute("registerSubscriber", request, response, payload, true);
  if (!s.ok()) return s;

  std::vector<std::string> pub_uris;
  for (int i = 0; i < payload.size(); i++) {
    if (payload[i] != this_server_uri) {
      pub_uris.push_back(std::string(payload[i]));
    }
  }

  return PubUpdate(topic, pub_uris);
}

Status ROSMasterProxy::RequestTopic(const std::string& pub_uri,
                                    const std::string& topic) {
  XmlRpc::XmlRpcValue request, response, payload;
  request[0] = ros::this_node::getName();
  request[1] = topic;
  request[2][0][0] = std::string("TCPROS");
  std::string peer_host;
  uint32_t peer_port;
  if (!ros::network::splitURI(pub_uri, peer_host, peer_port))
    return errors::InvalidArgument(
        base::StringPrintf("Invalid XMLRPC URI: %s.", pub_uri.c_str()));

  XmlRpc::XmlRpcClient* c =
      new XmlRpc::XmlRpcClient(peer_host.c_str(), peer_port, "/");
  Status s = Execute(c, "requestTopic", request, response, payload);
  if (!s.ok()) return s;

  if (payload.size() == 0)
    return errors::Aborted(base::StringPrintf(
        "No available protocol for topic: %s.", topic.c_str()));

  std::string protocol = payload[0];
  if (protocol == "TCPROS") {
    std::string pub_host = payload[1];
    int pub_port = payload[2];
    TopicInfo topic_info;
    topic_info.set_topic(base::StringPrintf("ros://%s", topic.c_str()));
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
    IPEndPoint* ip_endpoint = channel_def.mutable_ip_endpoint();
    ip_endpoint->set_ip(pub_host);
    ip_endpoint->set_port(pub_port);
    *topic_info.mutable_topic_source()->add_channel_defs() = channel_def;
    topic_info.set_ros_node_name(ros::this_node::getName());
    master_->NotifyAllSubscribers(topic_info);
    return Status::OK();
  } else if (protocol == "UDPROS") {
    return errors::Unimplemented(base::StringPrintf(
        "Not Implemented for UDPROS for topic: %s.", topic.c_str()));
  }
  return errors::Aborted(base::StringPrintf(
      "Unknown protocol %s for topic: %s.", protocol.c_str(), topic.c_str()));
}

void ROSMasterProxy::PubUpdateCallback(XmlRpc::XmlRpcValue& request,
                                       XmlRpc::XmlRpcValue& response) {
  std::vector<std::string> pub_uris;
  for (int idx = 0; idx < request[2].size(); idx++) {
    pub_uris.push_back(request[2][idx]);
  }

  Status s = PubUpdate(std::string(request[1]), pub_uris);
  if (s.ok()) {
    response = ros::xmlrpc::responseInt(1, "", 0);
  } else {
    response = ros::xmlrpc::responseInt(0, s.error_message(), 0);
  }
}

Status ROSMasterProxy::PubUpdate(const std::string& topic,
                                 const std::vector<std::string>& pub_uris) {
  if (pub_uris.size() == 0) {
    TopicInfo topic_info;
    topic_info.set_topic(base::StringPrintf("ros://%s", topic.c_str()));
    topic_info.set_status(TopicInfo::UNREGISTERED);
    master_->NotifyAllSubscribers(topic_info);
    return Status::OK();
  }

  Status s;
  for (const std::string& pub_uri : pub_uris) {
    s = RequestTopic(pub_uri, topic);
    if (s.ok()) return s;
  }
  return Status::OK();
}

}  // namespace felicia

#endif  // defined(HAS_ROS)