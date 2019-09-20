// @ts-ignore
import { Dropdown, Label } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FeliciaVizStore } from '../store';
import DepthCameraFrameViewState from '../store/ui/depth-camera-frame-view-state';
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

export interface Props {
  title: string;
  store?: FeliciaVizStore;
}

@inject('store')
@observer
export default class ColormapDropdown extends Component<Props> {
  constructor(props: Props) {
    super(props);

    this.data = COLORMAPS.reduce(
      (obj, v) => {
        obj[v] = v; // eslint-disable-line no-param-reassign
        return obj;
      },
      {} as { [key: string]: string }
    );
  }

  private _onFilterChange = (value: string): void => {
    const { store } = this.props;
    if (!store) return;
    const viewState = store.uiState.getActiveViewState() as DepthCameraFrameViewState;
    viewState.setFilter(value);
  };

  private data: { [key: string]: string };

  render(): JSX.Element | null {
    const { title, store } = this.props;
    if (!store) return null;
    const viewState = store.uiState.getActiveViewState() as DepthCameraFrameViewState;

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown value={viewState.filter} data={this.data} onChange={this._onFilterChange} />
      </PanelItemContainer>
    );
  }
}
