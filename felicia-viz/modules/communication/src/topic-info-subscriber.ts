// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { TOPIC_INFO } from '@felicia-viz/proto/messages/master-data';
import { SERVER_ADDRESS, WEBSOCKET_PORT } from 'typings/settings';
import Connection from './connection';
import Worker from './subscriber-webworker';

export default class TopicInfoSubscriber {
  connection: Connection;
  worker: Worker;

  constructor(serverAddr: string | null, onmessage: (this: Worker, ev: MessageEvent) => void) {
    let serverAddrUsed = serverAddr;
    if (!serverAddrUsed) {
      serverAddrUsed = `ws://${SERVER_ADDRESS}:${WEBSOCKET_PORT}`;
    }
    this.connection = new Connection(serverAddrUsed);
    this.worker = new Worker();
    this.worker.onmessage = onmessage;
  }

  initialize(): void {
    this.connection.initialize(
      (): void => {
        const data = JSON.stringify({
          type: TOPIC_INFO,
        });

        this.connection.ws!.send(data);
      },
      (event: MessageEvent): void => {
        this.worker.postMessage({
          type: TOPIC_INFO,
          data: event.data,
        });
      }
    );
  }

  close(): void {
    this.worker.terminate();
    this.connection.ws!.close();
  }
}
