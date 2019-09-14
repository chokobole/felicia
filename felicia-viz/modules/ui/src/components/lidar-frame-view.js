import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';

import LidarFrameMessage from '@felicia-viz/proto/messages/lidar-frame-message';

import ActivatableFloatPanel from './common/activatable-float-panel';
import ResizeDetector from './common/resize-detector';
import LidarFrameWorker from '../webworkers/lidar-frame-webworker';

class LidarFrameViewImpl extends Component {
  static propTypes = {
    width: PropTypes.string.isRequired,
    height: PropTypes.string.isRequired,
    frame: PropTypes.instanceOf(LidarFrameMessage),
  };

  static defaultProps = {
    frame: null,
  };

  state = {
    scale: 1,
  };

  componentDidMount() {
    this.worker = new LidarFrameWorker();

    this.worker.onmessage = event => {
      this._drawImageData(event.data);
    };
  }

  shouldComponentUpdate(nextProps) {
    const { frame } = this.props;

    if (frame !== nextProps.frame) {
      this._loadLidar(nextProps.frame);
      return true;
    }

    return false;
  }

  componentWillUnmount() {
    this.worker.terminate();
  }

  _onCanvasLoad = ref => {
    this.canvas = ref;
  };

  _onResize = entry => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

  _onWheel = event => {
    let { scale } = this.state;
    scale += event.deltaY * -0.001;
    this.setState({ scale });
  };

  _loadLidar(frame) {
    if (!this.canvas) {
      return;
    }

    if (!frame) return;

    const { width, height, scale } = this.state;
    const ctx = this.canvas.getContext('2d');

    this.worker.postMessage({
      imageData: ctx.getImageData(0, 0, width, height),
      frame,
      scale,
    });
  }

  _drawImageData(imageData) {
    if (!this.canvas) {
      return;
    }

    const { width, height } = this.state;
    const ctx = this.canvas.getContext('2d');
    this.canvas.width = width;
    this.canvas.height = height;
    ctx.putImageData(imageData, 0, 0);
  }

  render() {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return (
      <div style={style} onWheel={this._onWheel}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}

@inject('store')
@observer
export default class LidarFrameView extends Component {
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
        type='LidarFrameView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <LidarFrameViewImpl width={width} height={height} frame={frame} />
      </ActivatableFloatPanel>
    );
  }
}
