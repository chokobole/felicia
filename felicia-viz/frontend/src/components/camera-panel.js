import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import { Dropdown, FloatPanel, Label } from '@streetscape.gl/monochrome';

import ImageView from 'components/image-view';
import Subscriber from 'util/subscriber';
import Worker from 'util/subscriber-webworker';
import TYPES from 'common/connection-type';
import STORE from 'store';
import { CameraPanelState } from 'store/ui-state';
import { FLOAT_PANEL_STYLE } from 'custom-styles';

const TITLE_HEIGHT = 28;

@inject('store')
@observer
export default class CameraPanel extends Component {
  static propTypes = {
    instance: PropTypes.instanceOf(CameraPanelState).isRequired,
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

    const { instance } = this.props;

    this.cameraSubscriber.initialize(event => {
      this.worker.postMessage({
        source: 'subscribeCamera',
        id: instance.id,
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

  _onChange = value => {
    const { instance } = this.props;

    instance.selectTopic(value);
    this.cameraSubscriber.requestTopic(TYPES.Camera.name, value);
  };

  render() {
    const { panelState } = this.state;
    const { instance } = this.props;
    const { height } = panelState;
    const { uiState } = STORE;
    const cameraPanelState = uiState.findCameraPanel(instance.id);
    const { camera } = cameraPanelState;

    return (
      <FloatPanel
        {...panelState}
        {...this.floatPanelSettings}
        onUpdate={this._onUpdate}
        style={FLOAT_PANEL_STYLE}>
        <div
          style={{
            height: `${height}px`,
          }}>
          <ImageView frame={camera.frame} height={`${height - 120}px`} />
          <Dropdown
            value={instance.topic}
            data={{
              color: 'color',
              depth: 'depth',
            }}
            onChange={this._onChange}
          />
          <div style={{ height: '90px', padding: '5px' }}>
            <Label>width: {camera.frame ? camera.frame.width : ''}</Label>
            <Label>height: {camera.frame ? camera.frame.height : ''}</Label>
            <Label>frameRate: {camera.frame ? camera.frame.frameRate : ''}</Label>
            <Label>pixelFormat: {camera.frame ? camera.frame.pixelFormat : ''}</Label>
          </div>
        </div>
      </FloatPanel>
    );
  }
}
