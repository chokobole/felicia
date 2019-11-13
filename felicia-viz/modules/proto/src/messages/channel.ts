// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export interface IPEndPointProtobuf {
  ip: string;
  port: number;
}

export enum ChannelDefTypeProtobuf {
  CHANNEL_TYPE_NONE = 0,
  CHANNEL_TYPE_SHM = 1,
  CHANNEL_TYPE_UDS = 2,
  CHANNEL_TYPE_UDP = 4,
  CHANNEL_TYPE_TCP = 8,
  CHANNEL_TYPE_WS = 16,
}

export interface ChannelDefProtobuf {
  type: ChannelDefTypeProtobuf;
  ipEndpoint: IPEndPointProtobuf;
}

export interface ChannelSourceProtobuf {
  channelDefs: Array<ChannelDefProtobuf>;
}
