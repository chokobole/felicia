import { TOPIC_INFO } from '@felicia-viz/communication/proto-types';

import TOPIC_MAP from 'topic-map';

function handleTopicInfoMessage(connection) {
  const topics = [];
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

export default function handleMessage(connection, message) {
  /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["connection"] }] */
  let data;
  try {
    data = JSON.parse(message);
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

export function handleClose() {
  // Do nothing
}
