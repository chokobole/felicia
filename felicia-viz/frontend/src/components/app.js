import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import CameraPanel from 'components/camera-panel';
import ControlPanel from 'components/control-panel';
import ToolBar from 'components/tool-bar';

import 'stylesheets/main.scss';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.shape({
      currentTime: PropTypes.number,
    }).isRequired,
  };

  _renderCameraPanels() {
    const { store } = this.props;
    const { currentTime, uiState } = store;
    const { cameraPanelStates } = uiState;

    return (
      <React.Fragment>
        {cameraPanelStates.map(cameraPanelState => {
          return (
            <CameraPanel
              key={cameraPanelState.id}
              id={cameraPanelState.id}
              currentTime={currentTime}
            />
          );
        })}
      </React.Fragment>
    );
  }

  render() {
    return (
      <div id='container'>
        <ControlPanel />
        {this._renderCameraPanels()}
        <ToolBar />
      </div>
    );
  }
}
