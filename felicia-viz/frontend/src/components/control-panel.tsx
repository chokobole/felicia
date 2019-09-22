import { FeliciaVizStore } from '@felicia-viz/ui/store';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import UI_TYPES from 'store/ui/ui-types';
import styled from 'styled-components';

const Title = styled.h3`
  padding-left: 15px;
  text-align: start;
`;

@inject('store')
@observer
export default class ControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private _renderContent = (store: FeliciaVizStore, type: string | null): JSX.Element | null => {
    if (type === null) return null;
    const f = UI_TYPES[type].renderControlPanel;
    if (f) return f(store);
    return null;
  };

  render(): JSX.Element | null {
    const { store } = this.props;
    if (!store) return null;
    const { controlPanelType } = store.uiState;

    let className = '';
    if (controlPanelType !== null) {
      className = 'is-opened';
    }

    return (
      <div id='control-panel'>
        <header>
          <Title>Felicia Viz</Title>
        </header>

        <main className={className}>{this._renderContent(store, controlPanelType)}</main>
      </div>
    );
  }
}
