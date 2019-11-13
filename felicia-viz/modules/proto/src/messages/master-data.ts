// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* eslint import/prefer-default-export: "off" */
import FeliciaProtoRoot from '../felicia-proto-root';
import { ChannelDefProtobuf, ChannelSourceProtobuf } from './channel';

export const TOPIC_INFO = 'felicia.TopicInfo';
export const SERVICE_INFO = 'felicia.ServiceInfo';

export const ChannelDefType = FeliciaProtoRoot.lookupEnum('felicia.ChannelDef.Type');
export const TopicInfoStatus = FeliciaProtoRoot.lookupEnum('felicia.TopicInfo.Status');
export const ServiceInfoStatus = FeliciaProtoRoot.lookupEnum('felicia.ServiceInfo.Status');

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
  masterNotificationWatcherSource: ChannelSourceProtobuf;
  heartBeatDuration: number;
}

export enum TopicInfoStatusProtobuf {
  REGISTERED = 0,
  UNREGISTERED = 1,
}

export enum TopicInfoImplTypeProtobuf {
  PROTOBUF = 0,
  ROS = 1,
}

export interface TopicInfoProtobuf {
  topic: string;
  typeName: string;
  implType: TopicInfoImplTypeProtobuf;
  topicSource: ChannelSourceProtobuf;
  status: TopicInfoStatusProtobuf;
  rosNodeName: string;
}

export enum ServiceInfoStatusProtobuf {
  REGISTERED = 0,
  UNREGISTERED = 1,
}

export interface ServiceInfoProtobuf {
  service: string;
  typeName: string;
  serviceSource: ChannelSourceProtobuf;
  status: ServiceInfoStatusProtobuf;
}

export interface MasterNotification {
  topicInfo: TopicInfoProtobuf;
  serviceInfo: ServiceInfoProtobuf;
}

export interface ClientFilterProtobuf {
  all: boolean;
  id: number;
}

export interface NodeFilterProtobuf {
  all: boolean;
  publishingTopic: string;
  subscribingTopic: string;
  requestingService: string;
  servingService: string;
  name: string;
  watcher: boolean;
}

export interface TopicFilterProtobuf {
  all: boolean;
  topic: string;
}

export interface ServiceFilterProtobuf {
  all: boolean;
  service: string;
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
