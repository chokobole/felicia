import PROTO_TYPES, { TOPIC_INFO } from '@felicia-viz/communication';

import feliciaJs from 'felicia_js.node';
import TOPIC_MAP from 'topic-map';

function handleTopicInfoMessage(connection) {
  const topics = [];
  TOPIC_MAP.forEach(value => {
    const { typeName } = value;
    topics.push({
      topic: value.topic,
      typeName,
    });
  });

  connection.ws.send(
    JSON.stringify({
      data: topics,
    })
  );
}

function subscribeTopic(topic, ws) {
  if (!TOPIC_MAP.has(topic)) {
    console.error(`topicMap doesn't hold requested ${topic}.`);
    return;
  }

  const topicInfo = TOPIC_MAP.get(topic);
  console.log(topicInfo);

  feliciaJs.MasterProxy.subscribeTopic(
    topicInfo,
    message => {
      const { type } = message;
      const protoType = PROTO_TYPES[type];
      if (!protoType) {
        console.error(`Don't know how to handle the message type ${type} for topic ${topic}`);
        return;
      }

      const buffer = protoType.encode(message.message).finish();
      ws.broadcast(topic, buffer, type);
    },
    status => {
      console.error(`[OnSubscriptionError] for ${topic}`);
      console.error(status.errorMessage());
    },
    new feliciaJs.Settings({
      period: 100,
      queue_size: 1,
      isDynamicBuffer: true,
    })
  );
}

export default function handleMessage(connection, ws, message) {
  /* eslint no-param-reassign: ["error", { "props": true, "ignorePropertyModificationsFor": ["connection"] }] */
  let data;
  try {
    data = JSON.parse(message);
  } catch (e) {
    console.error(e);
    return;
  }

  const { type, topic } = data;
  connection.type = type;
  if (type === TOPIC_INFO) {
    handleTopicInfoMessage(connection);
    return;
  }

  connection.topic = topic;
  subscribeTopic(topic, ws);
}

export function handleClose(connection) {
  const { type, topic } = connection;
  if (type === null) return;
  if (type === TOPIC_INFO) return;

  feliciaJs.MasterProxy.unsubscribeTopic(topic, status => {
    console.log(`[OnUnsubscribeTopic] for ${topic}`);
    if (!status.ok()) {
      console.error(status.errorMessage());
    } else {
      console.log('Succesfully closed');
    }
  });
}
