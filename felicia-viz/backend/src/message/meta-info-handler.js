import QUERY_TYPES from 'common/query-type';
import TOPIC_MAP from 'topic-map';

export default function handleMetaInfoMessage(connection, data) {
  const { queryType } = data;
  if (queryType === QUERY_TYPES.Topics.name) {
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
        queryType,
        data: topics,
      })
    );
  }
}
