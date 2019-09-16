import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { NotificationContainer } from 'react-notifications';

import TopicInfoSubscriber from '@felicia-viz/communication/topic-info-subscriber';
// TODO: Make below as a plugin
// import {
//   ImageWithBoundingBoxesControlPanel,
//   ImageWithBoundingBoxesView,
//   ImageWithBoundingBoxesViewState,
//   ImageWithHumansControlPanel,
//   ImageWithHumansView,
//   ImageWithHumansViewState,
// } from '@felicia-viz/deeplearning';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
import SUBSCRIBER from '@felicia-viz/ui/store/subscriber';
import KeyBinding from '@felicia-viz/ui/util/key-binding';

import 'fonts/felicia-icons.css';
import 'react-notifications/lib/notifications.css';
import 'stylesheets/main.scss';

import CommandPanel from 'components/command-panel';
import ControlPanel from 'components/control-panel';
import MainScene from 'components/main-scene';
import ToolBar from 'components/tool-bar';
import UI_TYPES, { MainSceneType } from 'store/ui/ui-types';

@inject('store')
@observer
export default class App extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  state = {
    isCommandPanelVisible: false,
  };

  constructor(props) {
    super(props);

    // const ImageWithBoundingBoxesViewType = {
    //   name: 'ImageWithBoundingBoxesView',
    //   className: 'image-with-bounding-boxes-view',
    //   state: ImageWithBoundingBoxesViewState,
    //   renderView: id => {
    //     return <ImageWithBoundingBoxesView key={id} id={id} />;
    //   },
    //   renderControlPanel: () => {
    //     return <ImageWithBoundingBoxesControlPanel />;
    //   },
    // };

    // UI_TYPES[ImageWithBoundingBoxesViewType.name] = ImageWithBoundingBoxesViewType;

    // const ImageWithHumansViewType = {
    //   name: 'ImageWithHumansView',
    //   className: 'image-with-humans-view',
    //   state: ImageWithHumansViewState,
    //   renderView: id => {
    //     return <ImageWithHumansView key={id} id={id} />;
    //   },
    //   renderControlPanel: () => {
    //     return <ImageWithHumansControlPanel />;
    //   },
    // };

    // UI_TYPES[ImageWithHumansViewType.name] = ImageWithHumansViewType;

    const { store } = this.props;
    store.uiState.init(UI_TYPES, MainSceneType.name, SUBSCRIBER);
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

    this.keyBinding = new KeyBinding(document);
    this.keyBinding.bind();
    this.keyBinding.registerAction(['Control', 'P'], e => {
      this.setState({ isCommandPanelVisible: true });
      e.preventDefault();
    });
  }

  componentWillUnmount() {
    this.topicInfoSubscriber.close();

    this.keyBinding.unbind();
  }

  _onCommandPanelBlur = () => {
    this.setState({ isCommandPanelVisible: false });
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
    const { isCommandPanelVisible } = this.state;
    const { uiState } = store;
    const viewState = uiState.findView(0);
    const { occupancyGridMap, pose, pointcloudFrame } = viewState;

    return (
      <div id='container'>
        {isCommandPanelVisible && <CommandPanel onBlur={this._onCommandPanelBlur} />}
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
