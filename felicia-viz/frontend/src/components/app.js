import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';

import ControlPanel from 'components/control-panel';
import MainScene from 'components/main-scene';
import ToolBar from 'components/tool-bar';
import META_INFO_SUBSCRIBER from 'util/meta-info-subscriber';
import UI_TYPES from 'store/ui/ui-types';

import 'fonts/felicia-icons.css';
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
        store.uiState.activeViewState.unset();
        break;
      }
      default:
        break;
    }
  };

  _renderViews() {
    const { store } = this.props;

    return (
      <React.Fragment>
        {store.uiState.viewStates.map(viewState => {
          const uiType = UI_TYPES[viewState.type()];
          return uiType.renderView(viewState.id);
        })}
      </React.Fragment>
    );
  }

  render() {
    return (
      <div id='container'>
        <ControlPanel />
        <MainScene />
        {this._renderViews()}
        <ToolBar />
      </div>
    );
  }
}
