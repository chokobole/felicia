import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';

import { ActivatableFloatPanel } from '@felicia-viz/ui';

import { CameraFrameViewImpl } from './camera-frame-view';

@inject('store')
@observer
export default class DepthCameraFrameView extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    store: PropTypes.object.isRequired,
  };

  state = {
    width: '100%',
    height: 'auto',
  };

  _onResize = panelState => {
    const { width, height } = this.state;
    const w = `${panelState.width}px`;
    const h = `${panelState.height}px`;
    if (width === w && height === h) return;
    this.setState({ width: w, height: h });
  };

  render() {
    const { id, store } = this.props;
    const { width, height } = this.state;
    const { uiState } = store;
    const viewState = store.uiState.findView(id);
    const { frame, filter, frameToAlign } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='DepthCameraFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <CameraFrameViewImpl
          width={width}
          height={height}
          frame={frame}
          filter={filter}
          frameToAlign={frameToAlign}
        />
      </ActivatableFloatPanel>
    );
  }
}
