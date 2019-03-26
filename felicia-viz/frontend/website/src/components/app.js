import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import CameraPanel from './camera-panel';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.shape({
      currentTime: PropTypes.number
    }).isRequired
  };

  render() {
    const { store } = this.props;
    const { currentTime } = store;
    // this is experimental number
    const N = 5000;
    const images = Array.from(Array(N).keys()).map((_, index) => {
      return `images/image${index + 1}.jpg`;
    });

    return (
      <div id="container">
        <CameraPanel images={images} currentTime={currentTime} />
      </div>
    );
  }
}
