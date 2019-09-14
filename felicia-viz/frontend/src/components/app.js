import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { NotificationContainer } from 'react-notifications';

import TopicInfoSubscriber from '@felicia-viz/communication/topic-info-subscriber';

import ControlPanel from 'components/control-panel';
import MainScene from 'components/main-scene';
import ToolBar from 'components/tool-bar';
import UI_TYPES, { MainSceneType } from 'store/ui/ui-types';

import 'fonts/felicia-icons.css';
import 'react-notifications/lib/notifications.css';
import 'stylesheets/main.scss';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  constructor(props) {
    super(props);
    const { store } = this.props;
    const { uiState } = store;
    uiState.init(UI_TYPES, MainSceneType.name);
  }

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
      case 8:
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
    const { uiState } = store;

    return (
      <React.Fragment>
        {uiState.viewStates.map(viewState => {
          const type = viewState.viewType();
          const uiType = UI_TYPES[type];
          if (viewState.id === 0) return null;
          return uiType.renderView(viewState.id);
        })}
      </React.Fragment>
    );
  }

  render() {
    const { store } = this.props;
    const { uiState } = store;
    const viewState = uiState.findView(0);
    const { occupancyGridMap, pose, pointcloudFrame } = viewState;

    return (
      <div id='container'>
        <ControlPanel />
        <MainScene
          uiState={uiState}
          occupancyGridMap={occupancyGridMap}
          pose={pose}
          pointcloudFrame={pointcloudFrame}
        />
        {this._renderViews()}
        <ToolBar />
        <NotificationContainer />
      </div>
    );
  }
}
