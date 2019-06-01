import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { NotificationContainer } from 'react-notifications';

import { TopicInfoSubscriber } from '@felicia-viz/communication';

import ControlPanel from 'components/control-panel';
import MainScene from 'components/main-scene';
import ToolBar from 'components/tool-bar';
import UI_TYPES from 'store/ui/ui-types';

import 'fonts/felicia-icons.css';
import 'react-notifications/lib/notifications.css';
import 'stylesheets/main.scss';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  componentDidMount() {
    const { store } = this.props;

    this.topicInfoSubscriber = new TopicInfoSubscriber(null, event => {
      const { type, data } = event.data;
      store.update({
        type,
        data,
      });
    });
    this.topicInfoSubscriber.initialize();

    document.addEventListener('keydown', this._onKeyDown);
  }

  componentWillUnmount() {
    this.topicInfoSubscriber.close();

    document.removeEventListener('keydown', this._onKeyDown);
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
        <NotificationContainer />
      </div>
    );
  }
}
