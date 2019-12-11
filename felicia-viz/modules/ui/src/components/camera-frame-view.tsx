// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import CameraFrameMessage from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FeliciaVizStore } from '../store';
import CameraFrameViewState from '../store/ui/camera-frame-view-state';
import CameraFrameWorker, { OutputEvent } from '../webworkers/camera-frame-webworker';
import ActivatableFloatPanel, { PanelState } from './common/activatable-float-panel';
import ResizableCanvas from './common/resizable-canvas';

interface CameraFrameViewImplProps {
  width: string;
  height: string;
  frame: CameraFrameMessage | DepthCameraFrameMessage | null;
  filter: string;
  frameToAlign: CameraFrameMessage | null;
}

export class CameraFrameViewImpl extends Component<CameraFrameViewImplProps> {
  static defaultProps = {
    frame: null,
    filter: '',
    frameToAlign: null,
  };

  private proxyContext: CanvasRenderingContext2D | null = null;

  componentDidMount(): void {
    this.worker = new CameraFrameWorker();

    this.worker.onmessage = (event: OutputEvent): void => {
      this._drawImageData(event.data);
    };
  }

  shouldComponentUpdate(nextProps: CameraFrameViewImplProps): boolean {
    const { frame } = this.props;

    if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
      return true;
    }

    return false;
  }

  componentWillUnmount(): void {
    this.worker.terminate();
  }

  private _onCanvasLoad = (
    proxyCavnas: HTMLCanvasElement,
    proxyContext: CanvasRenderingContext2D | null,
    resizableCanvas: ResizableCanvas
  ): void => {
    this.proxyCanvas = proxyCavnas;
    this.proxyContext = proxyContext;
    this.resizableCanvas = resizableCanvas;
  };

  private worker: CameraFrameWorker;

  private proxyCanvas?: HTMLCanvasElement;

  private resizableCanvas?: ResizableCanvas;

  private _loadImageData(frame: CameraFrameMessage | DepthCameraFrameMessage | null): void {
    if (!(this.proxyContext && this.resizableCanvas)) return;

    if (!frame) {
      this.resizableCanvas.clearRect();
      return;
    }

    const { width, height } = frame.cameraFormat.size;
    const { filter, frameToAlign } = this.props; // for depth-camera
    const imageData = this.proxyContext.getImageData(0, 0, width, height);

    this.worker.postMessage({
      imageData: imageData,
      frame,
      filter,
      frameToAlign,
    }, [imageData.data.buffer, frame.data.buffer]);
  }

  private _drawImageData(imageData: ImageData): void {
    if (!(this.proxyCanvas && this.proxyContext && this.resizableCanvas)) {
      return;
    }

    this.proxyCanvas.width = imageData.width;
    this.proxyCanvas.height = imageData.height;

    this.proxyContext.putImageData(imageData, 0, 0);

    this.resizableCanvas.update();
  }

  render(): JSX.Element {
    const { width, height } = this.props;

    return <ResizableCanvas width={width} height={height} onCanvasLoad={this._onCanvasLoad} />;
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
export default class CameraFrameView extends Component<Props, State> {
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
    const { width, height } = this.state;
    if (!store) return null;
    const { uiState } = store;
    const viewState = uiState.findView(id);
    const { frame } = viewState as CameraFrameViewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='CameraFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <CameraFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
