import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import styled from 'styled-components';

import CameraControlPanel from 'components/camera-control-panel';
import { FeliciaVizStore } from 'store';
import { UI_TYPES } from 'store/ui-state';

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
    switch (type) {
      case UI_TYPES.CameraPanel.name: {
        return <CameraControlPanel />;
      }
      default:
        return null;
    }
  };

  render() {
    const { store } = this.props;
    const { id, type } = store.uiState.activeWindow;

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
