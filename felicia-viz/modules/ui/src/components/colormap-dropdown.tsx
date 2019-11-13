// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// @ts-ignore
import { Dropdown, Label } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import STORE from '../store';
import DepthCameraFrameViewState from '../store/ui/depth-camera-frame-view-state';
import { FormProps, PanelItemContainer } from './common/panel-item';

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

export default class ColormapDropdown extends Component<FormProps<string>> {
  constructor(props: FormProps<string>) {
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
    const viewState = STORE.uiState.getActiveViewState() as DepthCameraFrameViewState;
    viewState.setFilter(value);
  };

  private data: { [key: string]: string };

  render(): JSX.Element {
    const { title, value } = this.props;

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown value={value} data={this.data} onChange={this._onFilterChange} />
      </PanelItemContainer>
    );
  }
}
