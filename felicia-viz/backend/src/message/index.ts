import { TopicInfoProtobuf, TOPIC_INFO } from '@felicia-viz/proto/messages/master-data';
import WebSocket from 'ws';
import TOPIC_MAP from '../topic-map';
import Connection from '../websocket/connection';

function handleTopicInfoMessage(connection: Connection): void {
  const topics: Array<TopicInfoProtobuf> = [];
  TOPIC_MAP.forEach(value => {
    topics.push(value);
  });

  connection.ws.send(
    JSON.stringify({
      type: TOPIC_INFO,
      data: topics,
    })
  );
}

export default function handleMessage(connection: Connection, message: WebSocket.Data): void {
  /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["connection"] }] */
  let data;
  try {
    if (typeof message === 'string') data = JSON.parse(message);
  } catch (e) {
    console.error(e);
    return;
  }

  const { type } = data;
  connection.type = type;
  if (type === TOPIC_INFO) {
    handleTopicInfoMessage(connection);
  }
}

export function handleClose(): void {
  // Do nothing
}
