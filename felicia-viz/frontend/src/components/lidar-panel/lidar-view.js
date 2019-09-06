import React, { Component } from 'react';
import PropTypes from 'prop-types';

import { ResizeDetector } from '@felicia-viz/ui';

import { LidarFrameMessage } from 'store/ui/lidar-panel-state';
import Worker from 'util/lidar-frame-webworker.js';

export default class LidarView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    frame: PropTypes.instanceOf(LidarFrameMessage),
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
  };

  state = {
    scale: 1,
  };

  componentDidMount() {
    this.worker = new Worker();

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
