import Subscriber from '@felicia-viz/communication/subscriber';

import STORE from './index';

const SUBSCRIBER = new Subscriber(event => {
  const { data, type, destinations } = event.data;
  destinations.forEach(id => {
    STORE.update({
      type,
      data,
      id,
    });
  });
});

export default SUBSCRIBER;
