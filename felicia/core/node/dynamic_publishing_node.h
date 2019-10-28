#ifndef FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_
#define FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_

#include <memory>

#include "felicia/core/communication/dynamic_publisher.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class EXPORT DynamicPublishingNode : public NodeLifecycle {
 public:
  class EXPORT Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void OnDidCreate(DynamicPublishingNode* node) = 0;
    virtual void OnError(Status s) { LOG(ERROR) << s; }

    virtual void OnRequestPublish(Status s) { LOG_IF(ERROR, !s.ok()) << s; }

    virtual void OnRequestUnpublish(Status s) { LOG_IF(ERROR, !s.ok()) << s; }

    virtual void OnPublish(ChannelDef::Type type, Status s) {
      LOG_IF(ERROR, !s.ok()) << s << " from " << ChannelDef::Type_Name(type);
    }
  };

  explicit DynamicPublishingNode(std::unique_ptr<Delegate> delegate);

  ~DynamicPublishingNode();

  void OnDidCreate(NodeInfo node_info) override;

  void OnError(Status s) override;

  void RequestPublish(const std::string& topic_type, const std::string& topic,
                      int channel_defs,
                      const communication::Settings& settings);

  void RequestUnpublish(const std::string& topic);

  void PublishMessageFromJson(const std::string& json_message);

 private:
  void OnRequestPublish(Status s);

  void OnRequestUnpublish(Status s);

  std::unique_ptr<Delegate> delegate_;
  NodeInfo node_info_;

  std::unique_ptr<DynamicPublisher> publisher_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_