import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Dropdown, Label } from '@streetscape.gl/monochrome';

import { PanelItemContainer, failedToFindActiveState } from '@felicia-viz/ui';

const COLORMAPS = [
  'jet',
  'hsv',
  'hot',
  'cool',
  'spring',
  'summer',
  'autumn',
  'winter',
  'bone',
  'copper',
  'greys',
  'YIGnBu',
  'greens',
  'YIOrRd',
  'bluered',
  'RdBu',
  'picnic',
  'rainbow',
  'portland',
  'blackbody',
  'earth',
  'electric',
  'viridis',
  'inferno',
  'magma',
  'plasma',
  'warm',
  'cool',
  'rainbow-soft',
  'bathymetry',
  'cdom',
  'chlorophyll',
  'density',
  'freesurface-blue',
  'freesurface-red',
  'oxygen',
  'par',
  'phase',
  'salinity',
  'temperature',
  'turbidity',
  'velocity-blue',
  'velocity-green',
  'cubehelix',
];

@inject('store')
@observer
export default class ColormapDropdown extends Component {
  static propTypes = {
    title: PropTypes.string.isRequired,
    store: PropTypes.object.isRequired,
  };

  constructor(props) {
    super(props);

    this.data = COLORMAPS.reduce((obj, v) => {
      obj[v] = v; // eslint-disable-line no-param-reassign
      return obj;
    }, {});
  }

  _onFilterChange = value => {
    const { store } = this.props;
    const state = store.uiState.activeViewState.getState();
    if (state) {
      state.selectFilter(value);
    } else {
      failedToFindActiveState();
    }
  };

  render() {
    const { title, store } = this.props;
    const state = store.uiState.activeViewState.getState();
    let value = '';
    let isEnabled = false;
    if (state) {
      value = state.filter;
      if (state.frame) {
        isEnabled = state.frame.pixelFormat === 'PIXEL_FORMAT_Z16';
      }
    } else {
      failedToFindActiveState();
    }

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown
          value={value}
          data={this.data}
          onChange={this._onFilterChange}
          isEnabled={isEnabled}
        />
      </PanelItemContainer>
    );
  }
}
