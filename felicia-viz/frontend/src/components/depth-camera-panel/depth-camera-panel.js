import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { FloatPanel } from '@streetscape.gl/monochrome';

import { Activatable } from '@felicia-viz/ui';

import { FLOAT_PANEL_STYLE } from 'custom-styles';
import UI_TYPES from 'store/ui/ui-types';
import ImageView from 'components/camera-panel/image-view';
import PointcloudView from './pointcloud-view';

@inject('store')
@observer
export default class DepthCameraPanel extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    store: PropTypes.object.isRequired,
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
        height: panelState.width / videoAspectRatio,
      },
    });
  };

  render() {
    const { panelState } = this.state;
    const { id, store } = this.props;
    const { width, height } = panelState;
    const viewState = store.uiState.findView(id);
    const { frame, filter, pointcloudView } = viewState;

    return (
      <FloatPanel
        {...panelState}
        {...this.floatPanelSettings}
        onUpdate={this._onUpdate}
        style={FLOAT_PANEL_STYLE}>
        <Activatable id={id} type={UI_TYPES.DepthCameraPanel.name} uiState={store.uiState}>
          {pointcloudView ? (
            <PointcloudView
              frame={frame}
              width={`${width}px`}
              height={`${height}px`}
              filter={filter}
            />
          ) : (
            <ImageView frame={frame} height={`${height}px`} filter={filter} />
          )}
        </Activatable>
      </FloatPanel>
    );
  }
}
