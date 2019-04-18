import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import CameraPanel from 'components/camera-panel';
import TYPES from 'common/connection-type';
import Subscriber from 'util/subscriber';
import Worker from 'util/webworker';
import STORE from 'store';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.shape({
      currentTime: PropTypes.number,
    }).isRequired,
  };

  componentDidMount() {
    this.cameraSubscriber = new Subscriber();
    this.cameraSubscriber.initialize(TYPES.Camera.name, event => {
      STORE.update({
        frame: {
          width: 640,
          height: 480,
          data: event.data,
        },
      });
    });
    this.generalSubscriber = new Subscriber();
    this.worker = new Worker();
    this.generalSubscriber.initialize(TYPES.General.name, event => {
      this.worker.postMessage({
        source: 'subscriber',
        data: event.data,
      });
    });

    this.worker.onmessage = event => {
      console.log(event.data);
      // STORE.update(event.data);
    };
  }

  componentWillUnmount() {
    if (this.cameraSubscriber) {
      this.cameraSubscriber.close();
    }

    if (this.generalSubscriber) {
      this.generalSubscriber.close();
    }
  }

  render() {
    const { store } = this.props;
    const { currentTime, camera } = store;

    return <CameraPanel currentTime={currentTime} frame={camera.frame} />;
  }
}
