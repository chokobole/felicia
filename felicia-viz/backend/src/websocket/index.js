import WebSocket from 'ws';

import Connection from 'websocket/connection';

const connections = [];

export default onmessage => {
  const wss = new WebSocket.Server({ port: WEBSOCKET_PORT });

  wss.on('connection', ws => {
    connections.push(new Connection(ws, onmessage));
  });

  setInterval(() => {
    for (let i = 0; i < connections.length; i += 1) {
      if (connections[i].closed()) {
        connections.splice(i, 1);
      }
    }
  }, HEARTBEAT_INTERVAL);

  return {
    broadcast: (topic, data, type) => {
      connections.forEach(connection => {
        connection.send(topic, data, type);
      });
    },
  };
};
