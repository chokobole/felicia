// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import React, { PureComponent } from 'react';
import ResizeDetector from './resize-detector';

const PROXY = 'proxy';
const MAIN = 'main';

export interface Props {
  width?: string;
  height?: string;
  onCanvasLoad: (
    canvas: HTMLCanvasElement,
    ctx: CanvasRenderingContext2D | null,
    resizableCanvas: ResizableCanvas
  ) => void;
}

export interface State {
  width?: number;
  height?: number;
}

export default class ResizableCanvas extends PureComponent<Props, State> {
  static defaultProps = {
    width: '100%',
    height: 'auto',
  };

  private _onResize = (entry: ResizeObserverEntry): void => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

  private _onCanvasLoad(tag: string, ref: HTMLCanvasElement): void {
    if (!this._canvas) {
      this._canvas = {};
    }

    this._canvas[tag] = ref;

    if (!this._context) {
      this._context = {};
    }

    if (ref) {
      this._context[tag] = ref.getContext('2d');
    }

    const { onCanvasLoad } = this.props;
    if (tag === PROXY) {
      onCanvasLoad(this._canvas[PROXY], this._context[PROXY], this);
    }
  }

  private _canvas?: { [key: string]: HTMLCanvasElement };

  private _context?: { [key: string]: CanvasRenderingContext2D | null };

  clearRect(): void {
    if (!this._canvas) return;
    if (!this._context) return;

    const mainCanvas = this._canvas[MAIN];
    const mainContext = this._context[MAIN];

    if (!(mainCanvas && mainContext)) return;

    const { width, height } = mainCanvas;

    mainContext.clearRect(0, 0, width, height);
  }

  update(): void {
    if (!this._canvas) return;
    if (!this._context) return;

    const mainCanvas = this._canvas[MAIN];
    const mainContext = this._context[MAIN];
    const proxyCanvas = this._canvas[PROXY];

    if (!(mainCanvas && mainContext && proxyCanvas)) return;

    const { width, height } = this.state;
    if (!(width && height)) return;

    let finalHeight = height;
    // eslint-disable-next-line react/destructuring-assignment
    if (this.props.height === 'auto') {
      finalHeight = (width / proxyCanvas.width) * proxyCanvas.height;
    }

    mainCanvas.width = width;
    mainCanvas.height = height;
    mainContext.drawImage(proxyCanvas, 0, 0, width, finalHeight);
  }

  render(): JSX.Element {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad.bind(this, PROXY)} style={{ display: 'none' }} />
        <canvas ref={this._onCanvasLoad.bind(this, MAIN)} />
      </div>
    );
  }
}
