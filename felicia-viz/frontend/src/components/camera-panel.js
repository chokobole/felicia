import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { FloatPanel } from '@streetscape.gl/monochrome';

import Activatable from 'components/activatable';
import ImageView from 'components/image-view';
import { FLOAT_PANEL_STYLE } from 'custom-styles';
import { FeliciaVizStore } from 'store';
import { UI_TYPES } from 'store/ui-state';

const TITLE_HEIGHT = 28;

@inject('store')
@observer
export default class CameraPanel extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    currentTime: PropTypes.number,
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  static defaultProps = {
    currentTime: 0,
  };

  constructor(props) {
    super(props);

    this.floatPanelSettings = {
      movable: true,
      minimizable: false,
      resizable: true,
    };

    this.state = {
      panelState: {
        x: window.innerWidth - 500,
        y: 20,
        width: 480, // constant for a while. This should be modified in the future.
        height: 320, // constant for a while. This should be modified in the future.
      },
    };
  }

  _onUpdate = panelState => {
    // The constant videoAspectRatio should be provided from the external configuration
    // For now, let this value be constant.
    const videoAspectRatio = 1.333333;
    this.setState({
      panelState: {
        ...panelState,
        height: panelState.width / videoAspectRatio + TITLE_HEIGHT,
      },
    });
  };

  render() {
    const { panelState } = this.state;
    const { id, store } = this.props;
    const { height } = panelState;
    const cameraPanelState = store.uiState.findCameraPanel(id);
    const { camera, filter } = cameraPanelState;

    return (
      <FloatPanel
        {...panelState}
        {...this.floatPanelSettings}
        onUpdate={this._onUpdate}
        style={FLOAT_PANEL_STYLE}>
        <Activatable id={id} type={UI_TYPES.CameraPanel.name}>
          <ImageView frame={camera.frame} height={`${height}px`} filter={filter} />
        </Activatable>
      </FloatPanel>
    );
  }
}
