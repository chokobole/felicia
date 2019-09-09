import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import styled from 'styled-components';

import { FeliciaVizStore } from 'store';
import UI_TYPES from 'store/ui/ui-types';

const Title = styled.h3`
  padding-left: 15px;
  text-align: start;
`;

@inject('store')
@observer
export default class ControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  _renderContent = type => {
    if (type === null) return null;
    return UI_TYPES[type].renderControlView();
  };

  render() {
    const { store } = this.props;
    const { id, type } = store.uiState.activeViewState;

    let className = '';
    if (id !== null) {
      className = 'is-opened';
    }

    return (
      <div id='control-panel'>
        <header>
          <Title>Felicia Viz</Title>
        </header>

        <main className={className}>{this._renderContent(type)}</main>
      </div>
    );
  }
}
