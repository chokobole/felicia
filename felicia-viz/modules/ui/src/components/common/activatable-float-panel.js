import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { FloatPanel, withTheme } from '@streetscape.gl/monochrome';

import UIState from '../../store/ui-state';
import KeyBinding from '../../util/key-binding';
import Activatable from './activatable';

class ActivatableFloatPanel extends Component {
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
      focused: false,
      velocityY: 0,
      velocityX: 0,
    };
  }

  componentDidMount() {
    const { onUpdate } = this.props;
    onUpdate(this.state);
    this.keyBinding = new KeyBinding(this.ref);
    this.ref.focus();
  }

  _onUpdate = panelState => {
    const { onUpdate } = this.props;

    this.setState({
      ...panelState,
    });

    onUpdate(panelState);
  };

  _onFocus = () => {
    const { id, type, uiState } = this.props;
    uiState.activeViewState.set(id, type);

    const acc = 2;
    const maxVelocity = 30;
    this.setState({
      focused: true,
    });
    this.keyBinding.bind();
    this.keyBinding.registerAction(
      ['ArrowUp'],
      () => {
        const { y } = this.state;
        let { velocityY } = this.state;
        if (velocityY > 0) velocityY = 0;
        this.setState({
          y: Math.max(0, y + velocityY),
          velocityY: Math.max(velocityY - acc, -maxVelocity),
        });
      },
      () => {
        this.setState({ velocityY: 0 });
      }
    );
    this.keyBinding.registerAction(
      ['ArrowDown'],
      () => {
        const { height, y } = this.state;
        let { velocityY } = this.state;
        if (velocityY < 0) velocityY = 0;
        this.setState({
          y: Math.min(window.innerHeight - height, y + velocityY),
          velocityY: Math.min(velocityY + acc, maxVelocity),
        });
      },
      () => {
        this.setState({ velocityY: 0 });
      }
    );
    this.keyBinding.registerAction(
      ['ArrowLeft'],
      () => {
        const { x } = this.state;
        let { velocityX } = this.state;
        if (velocityX > 0) velocityX = 0;
        this.setState({
          x: Math.max(0, x + velocityX),
          velocityX: Math.max(velocityX - acc, -maxVelocity),
        });
      },
      () => {
        this.setState({ velocityX: 0 });
      }
    );
    this.keyBinding.registerAction(
      ['ArrowRight'],
      () => {
        const { width, x } = this.state;
        let { velocityX } = this.state;
        if (velocityX < 0) velocityX = 0;
        this.setState({
          x: Math.min(window.innerWidth - width, x + velocityX),
          velocityX: Math.min(velocityX + acc, maxVelocity),
        });
      },
      () => {
        this.setState({ velocityX: 0 });
      }
    );
    this.keyBinding.registerAction(
      navigator.platform.startsWith('Mac') ? ['Backspace'] : ['Delete'],
      () => {
        uiState.activeViewState.unset();
      }
    );
  };

  _onBlur = () => {
    this.setState({
      focused: false,
    });
    this.keyBinding.unbind();
  };

  _onRef = ref => {
    this.ref = ref;
  };

  render() {
    const { id, type, uiState, movable, minimizable, resizable, children } = this.props;
    const { focused, ...panelState } = this.state;

    return (
      <div
        role='tabpanel'
        tabIndex={id}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        ref={this._onRef}>
        <FloatPanel
          {...panelState}
          movable={movable}
          minimizable={minimizable}
          resizable={resizable}
          onUpdate={this._onUpdate}
          style={{
            wrapper: props => ({
              borderColor: focused ? props.theme.controlColorActive : props.theme.backgroundAlt,
              zIndex: 9999,
            }),
          }}>
          <Activatable id={id} type={type} uiState={uiState}>
            {children}
          </Activatable>
        </FloatPanel>
      </div>
    );
  }
}

export default withTheme(ActivatableFloatPanel);
