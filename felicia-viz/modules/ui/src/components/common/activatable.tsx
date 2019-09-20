import React, { PureComponent, ReactNode } from 'react';
import UIState from '../../store/ui-state';

export interface Props {
  id: number;
  type: string;
  width?: string;
  height?: string;
  children: ReactNode;
  uiState: UIState;
}

export default class Activatable extends PureComponent<Props> {
  static defaultProps = {
    width: '100%',
    height: '100%',
  };

  private _onPointerDown = (): void => {
    const { id, type, uiState } = this.props;
    uiState.markActive(id, type);
  };

  render(): JSX.Element {
    const { width, height, children } = this.props;

    return (
      <div style={{ width, height }} onPointerDown={this._onPointerDown} role='none'>
        {children}
      </div>
    );
  }
}
