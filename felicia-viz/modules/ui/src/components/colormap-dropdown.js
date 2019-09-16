import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Dropdown, Label } from '@streetscape.gl/monochrome';

import { FeliciaVizStore } from '../store';
import { PanelItemContainer } from './common/panel-item';

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
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
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
    const viewState = store.uiState.getActiveViewState();
    viewState.setFilter(value);
  };

  render() {
    const { title, store } = this.props;
    const viewState = store.uiState.getActiveViewState();

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown value={viewState.filter} data={this.data} onChange={this._onFilterChange} />
      </PanelItemContainer>
    );
  }
}
