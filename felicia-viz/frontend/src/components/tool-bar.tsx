// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import STORE from '@felicia-viz/ui/store';
import { UIType } from '@felicia-viz/ui/store/ui-state';
// @ts-ignore
import { Button, Popover, Tooltip } from '@streetscape.gl/monochrome';
import React, { PureComponent } from 'react';
import UI_TYPES from 'store/ui/ui-types';
import { TOOLBAR_BUTTON_STYLE, TOOLBAR_MENU_STYLE, TOOLTIP_STYLE } from '../custom-styles';

export default class Toolbar extends PureComponent {
  private _renderViewButton = (
    item: UIType,
    opts: {
      tooltip?: string;
      position?: any;
      onClick?: () => void;
    } = {}
  ): JSX.Element => {
    const {
      tooltip = item.name,
      position = Popover.TOP,
      onClick = (): void => {
        STORE.uiState.addView(item.name);
      },
    } = opts;

    return (
      <Tooltip key={item.name} content={tooltip} position={position} style={TOOLTIP_STYLE}>
        <Button type={Button.MUTED} style={TOOLBAR_BUTTON_STYLE} onClick={onClick}>
          <i className={`felicia-icons felicia-icons-${item.className}`} />
        </Button>
      </Tooltip>
    );
  };

  private _renderPanelSelector = (): JSX.Element => {
    return (
      <div className='menu'>
        {Object.values(UI_TYPES)
          .filter((item: UIType) => item.renderView)
          .map((item: UIType) => this._renderViewButton(item))}
      </div>
    );
  };

  render(): JSX.Element {
    return (
      <div id='toolbar'>
        <Popover
          content={this._renderPanelSelector}
          trigger={Popover.CLICK}
          arrowSize={0}
          style={TOOLBAR_MENU_STYLE}>
          <Button type={Button.MUTED} style={TOOLBAR_BUTTON_STYLE}>
            <i className='felicia-icons felicia-icons-plus' />
          </Button>
        </Popover>
      </div>
    );
  }
}
