// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// @ts-ignore
import { FloatPanel, withTheme } from '@streetscape.gl/monochrome';
import CSS from 'csstype';
import { Theme } from 'frontend/src/custom-styles';
import React, { Component, ReactNode } from 'react';
import UIState from '../../store/ui-state';
import KEY_BINDING from '../../util/key-binding';
import Activatable from './activatable';

export interface Props {
  id: number;
  type: string;
  uiState: UIState;
  movable?: boolean;
  minimizable?: boolean;
  resizable?: boolean;
  onUpdate: (state: PanelState) => void;
  children: ReactNode;
}

export interface PanelState {
  x: number;
  y: number;
  width: number;
  height: number;
}

export interface State extends PanelState {
  focused: boolean;
  velocityX: number;
  velocityY: number;
}

class ActivatableFloatPanel extends Component<Props, State> {
  static defaultProps = {
    movable: true,
    minimizable: false,
    resizable: true,
  };

  state = {
    x: window.innerWidth - 500,
    y: 20,
    width: 480,
    height: 320,
    focused: false,
    velocityY: 0,
    velocityX: 0,
  };

  private keyBindingIds: number[] | null = null;

  componentDidMount(): void {
    const { onUpdate } = this.props;
    onUpdate(this.state);
    if (this.ref) this.ref.focus();
  }

  componentWillUnmount(): void {
    this._releaseKeyBindings();
  }

  private _onUpdate = (panelState: PanelState): void => {
    const { onUpdate } = this.props;

    this.setState({
      ...panelState,
    });

    onUpdate(panelState);
  };

  private _onFocus = (): void => {
    const { id, type, uiState } = this.props;
    uiState.markActive(id, type);
    if (!this.ref) return;

    const acc = 2;
    const maxVelocity = 30;
    this.setState({
      focused: true,
    });
    KEY_BINDING.bind(this.ref);
    this.keyBindingIds = [];
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(
        ['ArrowUp'],
        () => {
          const { y } = this.state;
          let { velocityY } = this.state;
          if (velocityY > 0) velocityY = 0;
          this.setState({
            y: Math.max(0, y + velocityY),
            velocityY: Math.max(velocityY - acc, -maxVelocity),
          });
        },
        () => {
          this.setState({ velocityY: 0 });
        }
      )
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(
        ['ArrowDown'],
        () => {
          const { height, y } = this.state;
          let { velocityY } = this.state;
          if (velocityY < 0) velocityY = 0;
          this.setState({
            y: Math.min(window.innerHeight - height, y + velocityY),
            velocityY: Math.min(velocityY + acc, maxVelocity),
          });
        },
        () => {
          this.setState({ velocityY: 0 });
        }
      )
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(
        ['ArrowLeft'],
        () => {
          const { x } = this.state;
          let { velocityX } = this.state;
          if (velocityX > 0) velocityX = 0;
          this.setState({
            x: Math.max(0, x + velocityX),
            velocityX: Math.max(velocityX - acc, -maxVelocity),
          });
        },
        () => {
          this.setState({ velocityX: 0 });
        }
      )
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(
        ['ArrowRight'],
        () => {
          const { width, x } = this.state;
          let { velocityX } = this.state;
          if (velocityX < 0) velocityX = 0;
          this.setState({
            x: Math.min(window.innerWidth - width, x + velocityX),
            velocityX: Math.min(velocityX + acc, maxVelocity),
          });
        },
        () => {
          this.setState({ velocityX: 0 });
        }
      )
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(
        navigator.platform.startsWith('Mac') ? ['Backspace'] : ['Delete'],
        () => {
          uiState.markInactive();
        }
      )
    );
  };

  private _onBlur = (): void => {
    this.setState({
      focused: false,
    });
    this._releaseKeyBindings();
  };

  private _onRef = (ref: HTMLDivElement): void => {
    this.ref = ref;
  };

  private ref?: HTMLDivElement;

  private _releaseKeyBindings(): void {
    if (this.keyBindingIds) this.keyBindingIds.forEach(id => KEY_BINDING.unregisterAction(id));
    this.keyBindingIds = null;
    if (this.ref) KEY_BINDING.unbind(this.ref);
  }

  render(): JSX.Element {
    const { id, type, uiState, movable, minimizable, resizable, children } = this.props;
    const { focused, ...panelState } = this.state;

    return (
      <div
        role='tabpanel'
        tabIndex={id}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        ref={this._onRef}>
        <FloatPanel
          {...panelState}
          movable={movable}
          minimizable={minimizable}
          resizable={resizable}
          onUpdate={this._onUpdate}
          style={{
            wrapper: (props: { theme: Theme }): React.CSSProperties => ({
              borderColor: (focused
                ? props.theme.controlColorActive
                : props.theme.backgroundAlt) as CSS.ColorProperty,
              zIndex: 9999,
            }),
          }}>
          <Activatable id={id} type={type} uiState={uiState}>
            {children}
          </Activatable>
        </FloatPanel>
      </div>
    );
  }
}

export default withTheme(ActivatableFloatPanel);
