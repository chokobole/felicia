import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Vector3 } from '@babylonjs/core/Maths/math';
import { ArcRotateCamera } from '@babylonjs/core/Cameras/arcRotateCamera';
import '@babylonjs/core/Meshes/meshBuilder';

import ImuFrameMessage from '@felicia-viz/proto/messages/imu-frame-message';
import { ActivatableFloatPanel } from '@felicia-viz/ui';

import { babylonCanvasStyle } from 'custom-styles';
import { createAxis, createScene } from 'util/babylon-util';
import UI_TYPES from 'store/ui/ui-types';

class ImuFrameViewImpl extends Component {
  static propTypes = {
    width: PropTypes.string.isRequired,
    height: PropTypes.string.isRequired,
    frame: PropTypes.instanceOf(ImuFrameMessage),
  };

  static defaultProps = {
    frame: null,
  };

  componentDidMount() {
    const { engine, scene } = createScene(this.canvas);

    const camera = new ArcRotateCamera('camera', 0, 0, 0, Vector3.Zero(), scene);
    camera.position = new Vector3(0, 0, -10);
    camera.attachControl(this.canvas, true);

    this.localOrigin = createAxis(10, scene);

    engine.runRenderLoop(() => {
      scene.render();
    });
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;
    if (frame !== nextProps.frame) {
      const { orientation } = nextProps.frame;
      this.localOrigin.rotationQuaternion = orientation.toBabylonQuaternion();
      return true;
    }

    const { width, height } = this.props;
    if (width !== nextProps.width || height !== nextProps.height) {
      return true;
    }

    return false;
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  render() {
    return <canvas style={babylonCanvasStyle(this.props)} ref={this._onCanvasLoad} />;
  }
}

@inject('store')
@observer
export default class ImuFrameView extends Component {
  static propTypes = {
    id: PropTypes.number.isRequired,
    store: PropTypes.object.isRequired,
  };

  state = {
    width: '100%',
    height: 'auto',
  };

  _onResize = panelState => {
    const { width, height } = this.state;
    const w = `${panelState.width}px`;
    const h = `${panelState.height}px`;
    if (width === w && height === h) return;
    this.setState({ width: w, height: h });
  };

  render() {
    const { id, store } = this.props;
    const { width, height } = this.state;
    const { uiState } = store;
    const viewState = store.uiState.findView(id);
    const { frame } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type={UI_TYPES.ImuFrameView.name}
        uiState={uiState}
        onUpdate={this._onResize}>
        <ImuFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
