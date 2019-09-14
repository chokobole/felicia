import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';

import CameraFrameMessage from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage from '@felicia-viz/proto/messages/depth-camera-frame-message';

import ActivatableFloatPanel from './common/activatable-float-panel';
import ResizableCanvas from './common/resizable-canvas';
import CameraFrameWorker from '../webworkers/camera-frame-webworker';

export class CameraFrameViewImpl extends Component {
  static propTypes = {
    width: PropTypes.string.isRequired,
    height: PropTypes.string.isRequired,
    frame: PropTypes.oneOfType([
      PropTypes.instanceOf(CameraFrameMessage),
      PropTypes.instanceOf(DepthCameraFrameMessage),
    ]),
    filter: PropTypes.string,
    frameToAlign: PropTypes.instanceOf(CameraFrameMessage),
  };

  static defaultProps = {
    frame: null,
    filter: '',
    frameToAlign: null,
  };

  componentDidMount() {
    this.worker = new CameraFrameWorker();

    this.worker.onmessage = event => {
      this._drawImageData(event.data);
    };
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;

    if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    this.worker.terminate();
  }

  _onCanvasLoad = (proxyCavnas, proxyContext, resizableCanvas) => {
    this.proxyCanvas = proxyCavnas;
    this.proxyContext = proxyContext;
    this.resizableCanvas = resizableCanvas;
  };

  _loadImageData(frame) {
    if (!this.proxyContext) return;

    if (!frame) return;

    const { width, height } = frame.cameraFormat.size;
    const { filter, frameToAlign } = this.props; // for depth-camera

    this.worker.postMessage({
      imageData: this.proxyContext.getImageData(0, 0, width, height),
      frame,
      filter,
      frameToAlign,
    });
  }

  _drawImageData(imageData) {
    if (!this.proxyCanvas) {
      return;
    }

    if (!imageData) {
      this.resizableCanvas.clearRect();
      return;
    }

    this.proxyCanvas.width = imageData.width;
    this.proxyCanvas.height = imageData.height;

    this.proxyContext.putImageData(imageData, 0, 0);

    this.resizableCanvas.update();
  }

  render() {
    const { width, height } = this.props;

    return <ResizableCanvas width={width} height={height} onCanvasLoad={this._onCanvasLoad} />;
  }
}

@inject('store')
@observer
export default class CameraFrameView extends Component {
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
        type='CameraFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <CameraFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
