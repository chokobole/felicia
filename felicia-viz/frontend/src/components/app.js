import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import CameraPanel from 'components/camera-panel';
import SUBSCRIBER from 'store/subscriber';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.shape({
      currentTime: PropTypes.number,
    }).isRequired,
  };

  componentDidMount() {
    SUBSCRIBER.initialize();
  }

  render() {
    const { store } = this.props;
    const { currentTime } = store;

    return <CameraPanel currentTime={currentTime} />;
  }
}
