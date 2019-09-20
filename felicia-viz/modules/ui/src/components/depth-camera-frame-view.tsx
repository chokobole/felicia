import { ActivatableFloatPanel } from '@felicia-viz/ui';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FeliciaVizStore } from '../store';
import CameraFrameViewState from '../store/ui/camera-frame-view-state';
import DepthCameraFrameViewState from '../store/ui/depth-camera-frame-view-state';
import { CameraFrameViewImpl } from './camera-frame-view';
import { PanelState } from './common/activatable-float-panel';

export interface Props {
  id: number;
  store?: FeliciaVizStore;
}

export interface State {
  width: string;
  height: string;
}

@inject('store')
@observer
export default class DepthCameraFrameView extends Component<Props, State> {
  state = {
    width: '100%',
    height: 'auto',
  };

  private _onResize = (panelState: PanelState): void => {
    const { width, height } = this.state;
    const w = `${panelState.width}px`;
    const h = `${panelState.height}px`;
    if (width === w && height === h) return;
    this.setState({ width: w, height: h });
  };

  render(): JSX.Element | null {
    const { id, store } = this.props;
    if (!store) return null;
    const { width, height } = this.state;
    const { uiState } = store;
    const viewState = uiState.findView(id) as (CameraFrameViewState | DepthCameraFrameViewState);
    const { frame, filter, frameToAlign } = viewState as DepthCameraFrameViewState;

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
