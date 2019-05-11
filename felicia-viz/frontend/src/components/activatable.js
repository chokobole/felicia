import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

import STORE from 'store';

export default class Activatable extends PureComponent {
  static propTypes = {
    id: PropTypes.number.isRequired,
    type: PropTypes.string.isRequired,
    children: PropTypes.node.isRequired,
  };

  _onClick = () => {
    const { id, type } = this.props;

    console.log(`${id} is activated.`);
    STORE.uiState.activeWindow.activate(id, type);
  };

  render() {
    const { children } = this.props;

    return (
      <div id='asdf' onMouseDown={this._onClick} role='none'>
        {children}
      </div>
    );
  }
}
