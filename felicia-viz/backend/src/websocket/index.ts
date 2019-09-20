import { HEARTBEAT_INTERVAL, WEBSOCKET_PORT } from 'typings/settings';
import WebSocket from 'ws';
import Connection, { OnCloseCallback, OnMessgaeCallback } from './connection';

const connections: Array<Connection> = [];

export interface WebSocketInterface {
  broadcast: (data: any, type: string) => void;
}

export default (onmessage: OnMessgaeCallback, onclose: OnCloseCallback): WebSocketInterface => {
  const wss = new WebSocket.Server({ port: WEBSOCKET_PORT });

  wss.on('connection', ws => {
    connections.push(new Connection(ws, onmessage, onclose));
  });

  setInterval(() => {
    for (let i = 0; i < connections.length; i += 1) {
      if (connections[i].closed()) {
        connections.splice(i, 1);
      }
    }
  }, HEARTBEAT_INTERVAL);

  return {
    broadcast: (data: any, type: string): void => {
      connections.forEach(connection => {
        connection.send(data, type);
      });
    },
  };
};
