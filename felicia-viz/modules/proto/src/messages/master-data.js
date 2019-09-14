/* eslint import/prefer-default-export: "off" */
import FeliciaProtoRoot from '../felicia-proto-root';

export const TOPIC_INFO = 'felicia.TopicInfo';

export const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
export const TopicInfoStatus = FeliciaProtoRoot.lookupEnum('felicia.TopicInfo.Status');

export function findWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.find(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export function hasWSChannel(topicInfo) {
  return topicInfo.topicSource.channelDefs.some(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}
