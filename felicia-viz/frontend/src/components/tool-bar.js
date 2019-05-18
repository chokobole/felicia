import React, { PureComponent } from 'react';
import { Tooltip, Popover, Button } from '@streetscape.gl/monochrome';

import { TOOLTIP_STYLE, TOOLBAR_BUTTON_STYLE, TOOLBAR_MENU_STYLE } from 'custom-styles';
import STORE from 'store';
import UI_TYPES from 'store/ui/ui-types';

export default class Toolbar extends PureComponent {
  _renderPanelButton = (item, opts = {}) => {
    const {
      tooltip = item.name,
      position = Popover.TOP,
      onClick = () => {
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

  _renderPanelSelector = () => {
    return (
      <div className='menu'>
        {Object.values(UI_TYPES).map(item => this._renderPanelButton(item))}
      </div>
    );
  };

  render() {
    return (
      <div id='toolbar'>
        <Popover
          content={this._renderPanelSelector}
          trigger={Popover.CLICK}
          arrowSize={0}
          style={TOOLBAR_MENU_STYLE}>
          <Button type={Button.MUTED} style={TOOLBAR_BUTTON_STYLE} onClick={this._onClick}>
            <i className='felicia-icons felicia-icons-plus' />
          </Button>
        </Popover>
      </div>
    );
  }
}
