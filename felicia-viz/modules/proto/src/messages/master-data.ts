/* eslint import/prefer-default-export: "off" */
import FeliciaProtoRoot from '../felicia-proto-root';
import { ChannelDefProtobuf, ChannelSourceProtobuf } from './channel';

export const TOPIC_INFO = 'felicia.TopicInfo';

export const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
export const TopicInfoStatus = FeliciaProtoRoot.lookupEnum('felicia.TopicInfo.Status');

export interface NodeInfoProtobuf {
  name: string;
  clientId: number;
  watcher: boolean;
}

export interface PubSubTopicsProtobuf {
  publishingTopics: string[];
  subscribingTopics: string[];
}

export interface ClientInfoProtobuf {
  id: number;
  heartBeatSignallerSource: ChannelSourceProtobuf;
  topicInfoWatcherSource: ChannelSourceProtobuf;
  heartBeatDuration: number;
}

export enum TopicInfoStatusProtobuf {
  REGISTERED = 0,
  UNREGISTERED = 1,
}

export interface TopicInfoProtobuf {
  topic: string;
  typeName: string;
  topicSource: ChannelSourceProtobuf;
  status: TopicInfoStatusProtobuf;
}

export interface ClientFilterProtobuf {
  all: boolean;
  id: number;
}

export interface NodeFilterProtobuf {
  all: boolean;
  publishingTopic: string;
  subscribingTopic: string;
  name: string;
  watcher: boolean;
}

export interface TopicFilterProtobuf {
  all: boolean;
  topic: string;
}

export interface HeartBeatProtobuf {
  ok: boolean;
}

export function findWSChannel(topicInfo: TopicInfoProtobuf): ChannelDefProtobuf | undefined {
  return topicInfo.topicSource.channelDefs.find(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}

export function hasWSChannel(topicInfo: TopicInfoProtobuf): boolean {
  return topicInfo.topicSource.channelDefs.some(channelDef => {
    return ChannelDefType.valuesById[channelDef.type] === 'CHANNEL_TYPE_WS';
  });
}
