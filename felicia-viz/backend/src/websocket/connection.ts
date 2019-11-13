// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { HEARTBEAT_INTERVAL } from 'typings/settings';
import WebSocket from 'ws';

function noop(): void {}

export type OnMessgaeCallback = (connection: Connection, message: WebSocket.Data) => void;
export type OnCloseCallback = () => void;

export default class Connection {
  ws: WebSocket;

  type: string | null;

  isAlive?: boolean;

  constructor(ws: WebSocket, onmessage: OnMessgaeCallback, onclose: OnCloseCallback) {
    this.ws = ws;
    this.type = null;

    this.ws.on('message', message => {
      onmessage(this, message);
    });

    this.ws.on('close', () => {
      onclose();
    });

    this.ws.on('pong', () => {
      this.heartbeat();
    });
    this.heartbeat();
    this.checkHeartbeat();
  }

  send(data: any, type: string): void {
    if (this.ws.readyState === WebSocket.OPEN && type === this.type) {
      this.ws.send(data);
    }
  }

  heartbeat(): void {
    this.isAlive = true;
  }

  checkHeartbeat(): void {
    if (!this.isAlive) {
      this.ws.terminate();
      console.log('Failed to heartbeat, closing socket...');
      return;
    }

    this.isAlive = false;
    this.ws.ping(noop);
    setTimeout(this.checkHeartbeat.bind(this), HEARTBEAT_INTERVAL);
  }

  closed(): boolean {
    return this.ws.readyState === WebSocket.CLOSED;
  }
}
