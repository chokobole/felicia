import { Message } from '@felicia-viz/communication/subscriber';
import TopicInfoSubscriber from '@felicia-viz/communication/topic-info-subscriber';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
import SUBSCRIBER from '@felicia-viz/ui/store/subscriber';
import KEY_BINDING from '@felicia-viz/ui/util/key-binding';
import CommandPanel from 'components/command-panel';
import ControlPanel from 'components/control-panel';
import MainScene from 'components/main-scene';
import ToolBar from 'components/tool-bar';
import 'fonts/felicia-icons.css';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
// @ts-ignore
import { NotificationContainer } from 'react-notifications';
import 'react-notifications/lib/notifications.css';
import UI_TYPES, { MainSceneType } from 'store/ui/ui-types';
import 'stylesheets/main.scss';
import MainSceneState from '../store/ui/main-scene-state';

export interface Props {
  store?: FeliciaVizStore;
}

export interface State {
  isCommandPanelVisible: boolean;
}

@inject('store')
@observer
export default class App extends Component<Props, State> {
  state = {
    isCommandPanelVisible: false,
  };

  private keyBindingIds: number[] | null = null;

  constructor(props: Props) {
    super(props);

    // const ImageWithBoundingBoxesViewType: UIType = {
    //   name: 'ImageWithBoundingBoxesView',
    //   className: 'image-with-bounding-boxes-view',
    //   state: ImageWithBoundingBoxesViewState,
    //   renderView: (id: number) => {
    //     return <ImageWithBoundingBoxesView key={id} id={id} />;
    //   },
    //   renderControlPanel: () => {
    //     return <ImageWithBoundingBoxesControlPanel />;
    //   },
    // };

    // UI_TYPES[ImageWithBoundingBoxesViewType.name] = ImageWithBoundingBoxesViewType;

    // const ImageWithHumansViewType: UIType = {
    //   name: 'ImageWithHumansView',
    //   className: 'image-with-humans-view',
    //   state: ImageWithHumansViewState,
    //   renderView: (id: number) => {
    //     return <ImageWithHumansView key={id} id={id} />;
    //   },
    //   renderControlPanel: () => {
    //     return <ImageWithHumansControlPanel />;
    //   },
    // };

    // UI_TYPES[ImageWithHumansViewType.name] = ImageWithHumansViewType;

    const { store } = this.props;
    if (!store) return;
    store.uiState.init(UI_TYPES, MainSceneType.name, SUBSCRIBER);
  }

  componentDidMount(): void {
    const { store } = this.props;
    if (!store) return;

    this.topicInfoSubscriber = new TopicInfoSubscriber(
      null,
      (event: MessageEvent): void => {
        const { type, data } = event.data;
        store.update({
          type,
          data,
        } as Message);
      }
    );
    this.topicInfoSubscriber.initialize();

    KEY_BINDING.bind(document);
    this.keyBindingIds = [];
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(['Control', 'Shift', 'KeyP'], (e: KeyboardEvent) => {
        this.setState({ isCommandPanelVisible: true });
        e.preventDefault();
      })
    );
    this.keyBindingIds.push(
      KEY_BINDING.registerAction(['Control', 'KeyB'], () => {
        store.uiState.unsetControlPanel();
      })
    );
  }

  componentWillUnmount(): void {
    if (this.topicInfoSubscriber) this.topicInfoSubscriber.close();
    if (this.keyBindingIds) this.keyBindingIds.forEach(id => KEY_BINDING.unregisterAction(id));
    this.keyBindingIds = null;
    KEY_BINDING.unbind(document);
  }

  _onCommandPanelBlur = (): void => {
    this.setState({ isCommandPanelVisible: false });
  };

  private topicInfoSubscriber?: TopicInfoSubscriber;

  _renderViews(): JSX.Element | null {
    const { store } = this.props;
    if (!store) return null;

    return (
      <React.Fragment>
        {store.uiState.viewStates.map(viewState => {
          const type = viewState.viewType();
          const uiType = UI_TYPES[type];
          if (viewState.id === 0) return null;
          if (uiType.renderView) return uiType.renderView(viewState.id);
          return null;
        })}
      </React.Fragment>
    );
  }

  render(): JSX.Element | null {
    const { store } = this.props;
    if (!store) return null;
    const { isCommandPanelVisible } = this.state;
    const viewState = store.uiState.findView(0) as MainSceneState;
    const { camera, map, pose } = viewState;

    return (
      <div id='container'>
        {isCommandPanelVisible && <CommandPanel onBlur={this._onCommandPanelBlur} />}
        <ControlPanel />
        <MainScene camera={camera} map={map} pose={pose} />
        {this._renderViews()}
        <ToolBar />
        <NotificationContainer />
      </div>
    );
  }
}
