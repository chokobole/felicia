// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_
#define FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_

#include <ros/ros.h>

#include "third_party/chromium/base/no_destructor.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class RosMasterProxy {
 public:
  struct TopicType {
    TopicType();
    TopicType(const std::string& topic, const std::string& topic_type);

    std::string topic;
    std::string topic_type;
  };

  static RosMasterProxy& GetInstance();

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

  Status RegisterService(const std::string& service,
                         const IPEndPoint& ip_endpoint) const;

  Status UnregisterService(const std::string& service,
                           const IPEndPoint& ip_endpoint) const;

  Status LookupService(const std::string& service,
                       IPEndPoint* ip_endpoint) const;

 private:
  friend class base::NoDestructor<RosMasterProxy>;

  RosMasterProxy();
  ~RosMasterProxy();

  void PubUpdateCallback(XmlRpc::XmlRpcValue& request,
                         XmlRpc::XmlRpcValue& response) const;
  void RequestTopicCallback(XmlRpc::XmlRpcValue& request,
                            XmlRpc::XmlRpcValue& response) const;

  Status RequestTopic(const std::string& pub_uri,
                      const std::string& topic) const;

  Status PubUpdate(const std::string& topic,
                   const std::vector<std::string>& pub_uris) const;

  ros::XMLRPCManagerPtr xmlrpc_manager_;
  mutable Master* master_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_ROS_MASTER_PROXY_H_

#endif  // defined(HAS_ROS)