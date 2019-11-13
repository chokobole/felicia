// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import LidarFrameMessage from '@felicia-viz/proto/messages/lidar-frame-message';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FeliciaVizStore } from '../store';
import LidarFrameViewState from '../store/ui/lidar-frame-view-state';
import LidarFrameWorker, { OutputEvent } from '../webworkers/lidar-frame-webworker';
import ActivatableFloatPanel, { PanelState } from './common/activatable-float-panel';
import ResizeDetector from './common/resize-detector';

interface LidarFrameViewImplProps {
  width: string;
  height: string;
  frame: LidarFrameMessage | null;
}

interface LidarFrameViewImplState {
  width?: number;
  height?: number;
  scale: number;
}

class LidarFrameViewImpl extends Component<LidarFrameViewImplProps, LidarFrameViewImplState> {
  static defaultProps = {
    frame: null,
  };

  state: LidarFrameViewImplState = {
    scale: 1,
  };

  componentDidMount(): void {
    this.worker = new LidarFrameWorker();

    this.worker.onmessage = (event: OutputEvent): void => {
      this._drawImageData(event.data);
    };
  }

  shouldComponentUpdate(nextProps: LidarFrameViewImplProps): boolean {
    const { frame } = this.props;

    if (frame !== nextProps.frame) {
      this._loadLidar(nextProps.frame);
      return true;
    }

    return false;
  }

  componentWillUnmount(): void {
    this.worker.terminate();
  }

  private _onCanvasLoad = (ref: HTMLCanvasElement): void => {
    this.canvas = ref;
  };

  private _onResize = (entry: ResizeObserverEntry): void => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

  private _onWheel = (event: React.WheelEvent<HTMLDivElement>): void => {
    let { scale } = this.state;
    scale += event.deltaY * -0.001;
    this.setState({ scale });
  };

  private worker: LidarFrameWorker;

  private canvas?: HTMLCanvasElement;

  private _loadLidar(frame: LidarFrameMessage | null): void {
    if (!this.canvas) {
      return;
    }

    const { width, height, scale } = this.state;
    const ctx = this.canvas.getContext('2d');
    if (!(ctx && width && height)) return;

    if (!frame) {
      ctx.clearRect(0, 0, width, height);
      return;
    }

    this.worker.postMessage({
      imageData: ctx.getImageData(0, 0, width, height),
      frame,
      scale,
    });
  }

  private _drawImageData(imageData: ImageData): void {
    if (!this.canvas) {
      return;
    }

    const { width, height } = this.state;
    if (!(width && height)) return;

    const ctx = this.canvas.getContext('2d');
    if (!ctx) return;

    this.canvas.width = width;
    this.canvas.height = height;
    ctx.putImageData(imageData, 0, 0);
  }

  render(): JSX.Element {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return (
      <div style={style} onWheel={this._onWheel}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}

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
export default class LidarFrameView extends Component<Props, State> {
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
    const viewState = uiState.findView(id) as LidarFrameViewState;
    const { frame } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='LidarFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <LidarFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
