import React, { PureComponent } from 'react';
import { Tooltip, Popover, Button } from '@streetscape.gl/monochrome';

import { TOOLTIP_STYLE, TOOLBAR_BUTTON_STYLE } from 'custom-styles';
import STORE from 'store';

export default class Toolbar extends PureComponent {
  _onClick = () => {
    STORE.uiState.addCameraPanel();
  };

  render() {
    return (
      <div id='toolbar'>
        <Tooltip content='Camera' position={Popover.TOP} style={TOOLTIP_STYLE}>
          <Button type={Button.MUTED} style={TOOLBAR_BUTTON_STYLE} onClick={this._onClick}>
            <i className='material-icons'>photo_camera</i>
          </Button>
        </Tooltip>
      </div>
    );
  }
}
