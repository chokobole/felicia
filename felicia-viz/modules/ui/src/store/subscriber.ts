import Subscriber from '@felicia-viz/communication/subscriber';
import { OutputEvent } from '@felicia-viz/communication/subscriber-webworker';
import STORE from './index';

const SUBSCRIBER = new Subscriber(
  (event: OutputEvent): void => {
    const { data, type, destinations } = event.data;
    destinations.forEach((id: number) => {
      STORE.update({
        type,
        data,
        id,
      });
    });
  }
);

export default SUBSCRIBER;
