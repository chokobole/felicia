// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import { Vector3 } from '@babylonjs/core/Maths/math';
import '@babylonjs/core/Meshes/meshBuilder';
import { TransformNode } from '@babylonjs/core/Meshes/transformNode';
import ImuFrameMessage from '@felicia-viz/proto/messages/imu-frame-message';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { babylonCanvasStyle, SizeProps } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import ImuFrameViewState from '../store/ui/imu-frame-view-state';
import { createAxis, createScene } from '../util/babylon-util';
import ActivatableFloatPanel, { PanelState } from './common/activatable-float-panel';

interface ImuFrameViewImplProps {
  width: string;
  height: string;
  frame: ImuFrameMessage | null;
}

class ImuFrameViewImpl extends Component<ImuFrameViewImplProps> {
  static defaultProps = {
    frame: null,
  };

  componentDidMount(): void {
    if (!this.canvas) return;

    const { engine, scene } = createScene(this.canvas);

    const camera = new ArcRotateCamera('camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, 0, -10);
    camera.attachControl(this.canvas, true);

    this.localOrigin = createAxis(10, scene);

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps: ImuFrameViewImplProps): boolean {
    const { frame } = this.props;
    if (nextProps.frame && frame !== nextProps.frame) {
      const { orientation } = nextProps.frame;
      if (this.localOrigin) {
        this.localOrigin.rotationQuaternion = orientation.toBabylonQuaternion();
        return true;
      }
    }

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  private _onCanvasLoad = (ref: HTMLCanvasElement): void => {
    this.canvas = ref;
  };

  private canvas?: HTMLCanvasElement;

  private localOrigin?: TransformNode;

  render(): JSX.Element {
    return <canvas style={babylonCanvasStyle(this.props as SizeProps)} ref={this._onCanvasLoad} />;
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
export default class ImuFrameView extends Component<Props, State> {
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
    const viewState = uiState.findView(id) as ImuFrameViewState;
    const { frame } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='ImuFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <ImuFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
