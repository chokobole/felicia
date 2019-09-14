import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { FloatPanel } from '@streetscape.gl/monochrome';

import UIState from '../../store/ui-state';
import Activatable from './activatable';

export default class ActivatableFloatPanel extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    type: PropTypes.string.isRequired,
    uiState: PropTypes.instanceOf(UIState).isRequired,
    movable: PropTypes.bool,
    minimizable: PropTypes.bool,
    resizable: PropTypes.bool,
    onUpdate: PropTypes.func.isRequired,
    children: PropTypes.node.isRequired,
  };

  static defaultProps = {
    movable: true,
    minimizable: false,
    resizable: true,
  };

  constructor(props) {
    super(props);

    this.state = {
      x: window.innerWidth - 500,
      y: 20,
      width: 480,
      height: 320,
    };
  }

  componentDidMount() {
    const { onUpdate } = this.props;
    onUpdate(this.state);
  }

  _onUpdate = panelState => {
    const { onUpdate } = this.props;

    this.setState({
      ...panelState,
    });

    onUpdate(panelState);
  };

  render() {
    const { id, type, uiState, movable, minimizable, resizable, children } = this.props;

    return (
      <FloatPanel
        {...this.state}
        movable={movable}
        minimizable={minimizable}
        resizable={resizable}
        onUpdate={this._onUpdate}
        style={{
          wrapper: {
            zIndex: 9999,
          },
        }}>
        <Activatable id={id} type={type} uiState={uiState}>
          {children}
        </Activatable>
      </FloatPanel>
    );
  }
}
