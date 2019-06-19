import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { FloatPanel } from '@streetscape.gl/monochrome';

import { Activatable } from '@felicia-viz/ui';

import { FLOAT_PANEL_STYLE } from 'custom-styles';
import { panelInitialState, FLOAT_PANEL_SETTINGS } from 'custom-settings';
import UI_TYPES from 'store/ui/ui-types';
import PointcloudView from './pointcloud-view';

@inject('store')
@observer
export default class PointcloudPanel extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    store: PropTypes.object.isRequired,
  };

  constructor(props) {
    super(props);

    this.floatPanelSettings = FLOAT_PANEL_SETTINGS;
    this.state = { ...panelInitialState(window) };
  }

  _onUpdate = panelState => {
    this.setState({
      panelState: {
        ...panelState,
      },
    });
  };

  render() {
    const { panelState } = this.state;
    const { id, store } = this.props;
    const { width, height } = panelState;
    const viewState = store.uiState.findView(id);
    const { frame, filter, frameToAlign } = viewState;

    return (
      <FloatPanel
        {...panelState}
        {...this.floatPanelSettings}
        onUpdate={this._onUpdate}
        style={FLOAT_PANEL_STYLE}>
        <Activatable id={id} type={UI_TYPES.PointcloudPanel.name} uiState={store.uiState}>
          <PointcloudView
            frame={frame}
            width={`${width}px`}
            height={`${height}px`}
            filter={filter}
            frameToAlign={frameToAlign}
          />
        </Activatable>
      </FloatPanel>
    );
  }
}
