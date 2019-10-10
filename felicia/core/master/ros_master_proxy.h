#if defined(HAS_ROS)

#ifndef FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_
#define FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_

#include <ros/ros.h>

#include "third_party/chromium/base/no_destructor.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master.h"

namespace felicia {

class ROSMasterProxy {
 public:
  struct TopicType {
    TopicType();
    TopicType(const std::string& topic, const std::string& topic_type);

    std::string topic;
    std::string topic_type;
  };

  static ROSMasterProxy& GetInstance();

  Status Init(Master* master);

  Status Start();

  Status Shutdown();

  Status GetPid() const;

  Status GetPublishedTopicTypes(std::vector<TopicType>* topic_types) const;

  Status GetPublishedTopicType(const std::string& topic,
                               std::string* topic_type) const;

  Status RegisterPublisher(const std::string& topic,
                           const std::string& topic_type) const;

  Status RegisterSubscriber(const std::string& topic,
                            const std::string& topic_type) const;

  Status UnregisterPublisher(const std::string& topic) const;

  Status UnregisterSubscriber(const std::string& topic) const;

  Status RequestTopic(const std::string& pub_uri,
                      const std::string& topic) const;

 private:
  friend class base::NoDestructor<ROSMasterProxy>;

  ROSMasterProxy();
  ~ROSMasterProxy();

  void PubUpdateCallback(XmlRpc::XmlRpcValue& request,
                         XmlRpc::XmlRpcValue& response) const;
  void RequestTopicCallback(XmlRpc::XmlRpcValue& request,
                            XmlRpc::XmlRpcValue& response) const;

  Status PubUpdate(const std::string& topic,
                   const std::vector<std::string>& pub_uris) const;

  ros::XMLRPCManagerPtr xmlrpc_manager_;
  mutable Master* master_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_

#endif  // defined(HAS_ROS)