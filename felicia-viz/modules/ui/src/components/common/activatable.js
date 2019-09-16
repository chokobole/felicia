import PropTypes from 'prop-types';
import React, { PureComponent } from 'react';

import UIState from '../../store/ui-state';

export default class Activatable extends PureComponent {
  static propTypes = {
    id: PropTypes.number.isRequired,
    type: PropTypes.string.isRequired,
    width: PropTypes.string,
    height: PropTypes.string,
    children: PropTypes.node.isRequired,
    uiState: PropTypes.instanceOf(UIState).isRequired,
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
  };

  _onPointerDown = () => {
    const { id, type, uiState } = this.props;

    console.log(`${id} is activated.`);
    uiState.activeViewState.set(id, type);
  };

  render() {
    const { width, height, children } = this.props;

    return (
      <div style={{ width, height }} onPointerDown={this._onPointerDown} role='none'>
        {children}
      </div>
    );
  }
}
