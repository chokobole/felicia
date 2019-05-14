import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import CameraPanel from 'components/camera-panel';
import ControlPanel from 'components/control-panel';
import ToolBar from 'components/tool-bar';
import META_INFO_SUBSCRIBER from 'util/meta-info-subscriber';

import 'stylesheets/main.scss';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.shape({
      currentTime: PropTypes.number,
    }).isRequired,
  };

  componentDidMount() {
    document.addEventListener('keydown', this._onKeyDown);
    META_INFO_SUBSCRIBER.initialize();
  }

  componentWillUnmount() {
    document.removeEventListener('keydown', this._onKeyDown);
    META_INFO_SUBSCRIBER.close();
  }

  _onKeyDown = e => {
    switch (e.keyCode) {
      case 46: {
        // Delete
        const { store } = this.props;
        store.uiState.activeWindow.deactivate();
        break;
      }
      default:
        break;
    }
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
