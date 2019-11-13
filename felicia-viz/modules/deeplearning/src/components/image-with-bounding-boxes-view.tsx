// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { ImageWithBoundingBoxesMessage } from '@felicia-viz/proto/messages/bounding-box';
import { ActivatableFloatPanel, ResizableCanvas } from '@felicia-viz/ui';
import { PanelState } from '@felicia-viz/ui/components/common/activatable-float-panel';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
import ImageWorker, { OutputEvent } from '@felicia-viz/ui/webworkers/image-webworker';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import ImageWithBoundingBoxesViewState from '../store/image-with-bounding-boxes-view-state';

export interface ImageWithBoundingBoxesViewImplProps {
  width: string;
  height: string;
  frame: ImageWithBoundingBoxesMessage | null;
  fontSize: number;
  lineWidth: number;
  threshold: number;
}

class ImageWithBoundingBoxesViewImpl extends Component<ImageWithBoundingBoxesViewImplProps> {
  static defaultProps = {
    frame: null,
    fontSize: 15,
    lineWidth: 10,
    threshold: 0.5,
  };

  private proxyContext: CanvasRenderingContext2D | null = null;

  componentDidMount(): void {
    this.worker = new ImageWorker();

    this.worker.onmessage = (event: OutputEvent<ImageWithBoundingBoxesMessage>): void => {
      this._drawImageDataAndBoundingBoxes(event.data);
    };
  }

  shouldComponentUpdate(nextProps: ImageWithBoundingBoxesViewImplProps): boolean {
    const { frame, lineWidth, fontSize, threshold } = this.props;

    if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
      return true;
    }

    if (
      lineWidth !== nextProps.lineWidth ||
      fontSize !== nextProps.fontSize ||
      threshold !== nextProps.threshold
    ) {
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

  private worker: ImageWorker;

  private proxyCanvas?: HTMLCanvasElement;

  private resizableCanvas?: ResizableCanvas;

  private _loadImageData(frame: ImageWithBoundingBoxesMessage | null): void {
    if (!(this.proxyContext && this.resizableCanvas)) {
      return;
    }

    if (!frame) {
      this.resizableCanvas.clearRect();
      return;
    }

    const { image, boundingBoxes } = frame;
    const { width, height } = image.size;

    this.worker.postMessage({
      imageData: this.proxyContext.getImageData(0, 0, width, height),
      image,
      data: {
        boundingBoxes,
      },
    });
  }

  private _drawImageDataAndBoundingBoxes(data: {
    imageData: ImageData;
    data: ImageWithBoundingBoxesMessage;
  }): void {
    if (!(this.proxyContext && this.proxyCanvas && this.resizableCanvas)) {
      return;
    }

    if (!data) {
      this.resizableCanvas.clearRect();
      return;
    }

    const { imageData } = data;
    const { boundingBoxes } = data.data;
    const { width, height } = imageData;
    const { lineWidth, fontSize, threshold } = this.props;

    this.proxyCanvas.width = width;
    this.proxyCanvas.height = height;

    this.proxyContext.putImageData(imageData, 0, 0);
    for (let i = 0; i < boundingBoxes.length; i += 1) {
      const { color, box, label, score } = boundingBoxes[i];
      if (score >= threshold) {
        const { r, g, b } = color;
        const { topLeft, bottomRight } = box;
        const { x, y } = topLeft;
        const w = bottomRight.x - x;
        const h = bottomRight.y - y;
        const colorStyle = `rgb(${r}, ${g}, ${b})`;
        this.proxyContext.beginPath();
        this.proxyContext.font = `${fontSize}px sans`;
        this.proxyContext.fillStyle = colorStyle;
        this.proxyContext.strokeStyle = colorStyle;
        this.proxyContext.lineWidth = lineWidth;
        this.proxyContext.fillText(label, x, y);
        this.proxyContext.rect(x + lineWidth / 2, y + lineWidth / 2, w - lineWidth, h - lineWidth);
        this.proxyContext.stroke();
      }
    }

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
export default class ImageWithBoundingBoxesView extends Component<Props, State> {
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
    const viewState = uiState.findView(id) as ImageWithBoundingBoxesViewState;
    const { frame, lineWidth, threshold } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='ImageWithBoundingBoxesView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <ImageWithBoundingBoxesViewImpl
          width={width}
          height={height}
          frame={frame}
          lineWidth={lineWidth}
          threshold={threshold}
        />
      </ActivatableFloatPanel>
    );
  }
}
