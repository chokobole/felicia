import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import { FloatPanel } from '@streetscape.gl/monochrome';

import ImageView from 'components/image-view';
import Subscriber from 'util/subscriber';
import Worker from 'util/subscriber-webworker';
import TYPES from 'common/connection-type';
import STORE from 'store';
import { FLOAT_PANEL_STYLE } from './custom-styles';

const TITLE_HEIGHT = 28;

@inject('store')
@observer
export default class CameraPanel extends Component {
  static propTypes = {
    // User configuration
    currentTime: PropTypes.number,
  };

  static defaultProps = {
    currentTime: 0,
  };

  constructor(props) {
    super(props);

    this.floatPanelSettings = {
      movable: true,
      minimizable: false,
      resizable: true,
    };

    this.state = {
      panelState: {
        x: window.innerWidth - 660,
        y: 20,
        width: 640, // constant for a while. This should be modified in the future.
        height: 480, // constant for a while. This should be modified in the future.
      },
    };
  }

  componentDidMount() {
    this.cameraSubscriber = new Subscriber();
    this.worker = new Worker();
    this.cameraSubscriber.initialize(TYPES.Camera.name, event => {
      this.worker.postMessage({
        source: 'subscribeCamera',
        data: event.data,
      });
    });

    this.worker.onmessage = event => {
      STORE.update(event.data);
    };
  }

  componentWillUnmount() {
    if (this.cameraSubscriber) {
      this.cameraSubscriber.close();
    }
  }

  _onUpdate = panelState => {
    // The constant videoAspectRatio should be provided from the external configuration
    // For now, let this value be constant.
    const videoAspectRatio = 1.333333;
    this.setState({
      panelState: {
        ...panelState,
        height: panelState.width / videoAspectRatio + TITLE_HEIGHT,
      },
    });
  };

  render() {
    const { camera } = STORE;
    const { panelState } = this.state;
    const { width, height } = panelState;

    return (
      <FloatPanel
        {...panelState}
        {...this.floatPanelSettings}
        onUpdate={this._onUpdate}
        style={FLOAT_PANEL_STYLE}>
        <ImageView frame={camera.frame} canvasWidth={width} canvasHeight={height} />
      </FloatPanel>
    );
  }
}
