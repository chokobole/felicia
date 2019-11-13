// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {
  TopicInfoProtobuf,
  TopicInfoStatus,
  TOPIC_INFO,
} from '@felicia-viz/proto/messages/master-data';
// @ts-ignore
import feliciaJs from 'felicia_js.node';
import { isWin } from 'lib/environment';
import MasterClient from 'master-client';
import handleMessage, { handleClose } from 'message';
import TOPIC_MAP from 'topic-map';
import websocket from 'websocket';
import WebSocket from 'ws';
import Connection from './websocket/connection';

interface Status {
  errorMessage: () => string;
}

export default (): void => {
  const ws = websocket(
    (connection: Connection, message: WebSocket.Data) => {
      handleMessage(connection, message);
    },
    () => {
      handleClose();
    }
  );
  feliciaJs.MasterProxy.setBackground();

  if (isWin) {
    (global as any).MasterClient = MasterClient;
    feliciaJs.MasterProxy.startMasterClient();
  }

  const s = feliciaJs.MasterProxy.start();
  if (!s.ok()) {
    console.error(s.errorMessage());
    process.exit(1);
  }

  function requestRegisterTopicInfoWatcherNode(): void {
    if (isWin) {
      if (!feliciaJs.MasterProxy.isClientInfoSet()) {
        setTimeout(requestRegisterTopicInfoWatcherNode, 1000);
        return;
      }
    }

    feliciaJs.MasterProxy.requestRegisterTopicInfoWatcherNode(
      (message: { message: TopicInfoProtobuf }) => {
        console.log(`[TOPIC] : ${JSON.stringify(message.message)}`);
        const topicInfo = message.message;
        if (topicInfo.status === TopicInfoStatus.values.REGISTERED) {
          TOPIC_MAP.set(topicInfo.topic, topicInfo);
        } else {
          TOPIC_MAP.delete(topicInfo.topic);
        }
        const topics: Array<TopicInfoProtobuf> = [];
        TOPIC_MAP.forEach(value => {
          topics.push(value);
        });

        ws.broadcast(
          JSON.stringify({
            type: TOPIC_INFO,
            data: topics,
          }),
          TOPIC_INFO
        );
      },
      // eslint-disable-next-line no-shadow
      (s: Status) => {
        console.error(s.errorMessage());
        process.exit(1);
      }
    );
  }

  requestRegisterTopicInfoWatcherNode();

  feliciaJs.MasterProxy.run();
};
