import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

export default class Activatable extends PureComponent {
  static propTypes = {
    id: PropTypes.number.isRequired,
    type: PropTypes.string.isRequired,
    children: PropTypes.node.isRequired,
    uiState: PropTypes.object.isRequired,
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
