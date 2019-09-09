import PropTypes from 'prop-types';
import React, { PureComponent } from 'react';

import UIState from '../../ui-state';

export default class Activatable extends PureComponent {
  static propTypes = {
    id: PropTypes.number.isRequired,
    type: PropTypes.string.isRequired,
    children: PropTypes.node.isRequired,
    uiState: PropTypes.instanceOf(UIState).isRequired,
  };

  _onMouseDown = () => {
    const { id, type, uiState } = this.props;

    console.log(`${id} is activated.`);
    uiState.activeViewState.set(id, type);
  };

  render() {
    const { children } = this.props;

    return (
      <div onMouseDown={this._onMouseDown} role='none'>
        {children}
      </div>
    );
  }
}
