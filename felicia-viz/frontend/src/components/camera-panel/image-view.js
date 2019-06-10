import React, { Component } from 'react';
import PropTypes from 'prop-types';

import { ResizableCanvas } from '@felicia-viz/ui';

import { CameraFrame } from 'store/ui/camera-panel-state';
import Worker from 'util/image-view-webworker.js';

export default class ImageView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    src: PropTypes.string,
    frame: PropTypes.instanceOf(CameraFrame),
    filter: PropTypes.string,
    frameToAlign: PropTypes.instanceOf(CameraFrame),
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    src: '',
    frame: null,
    filter: '',
    frameToAlign: null,
  };

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      this._drawImageOrImageData(event.data);
    };
  }

  shouldComponentUpdate(nextProps) {
    const { src, frame } = this.props;

    if (src !== nextProps.src) {
      this._loadImage(nextProps.src);
      return true;
    }

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

  _loadImage(src) {
    if (src) {
      const image = new Image();
      image.src = src;
      image.addEventListener(
        'load',
        () => {
          this._drawImageOrImageData(image);
        },
        false
      );
    }
  }

  _loadImageData(frame) {
    if (!this.proxyContext) return;

    if (!frame) return;

    const { width, height, data, pixelFormat, converted } = frame;
    const { filter, frameToAlign } = this.props; // for depth-camera

    this.worker.postMessage({
      imageData: this.proxyContext.getImageData(0, 0, width, height),
      width,
      height,
      data,
      pixelFormat,
      converted,
      filter,
      frameToAlign,
    });
  }

  _drawImageOrImageData(image) {
    if (!this.proxyCanvas) {
      return;
    }

    if (!image) {
      this.resizableCanvas.clearRect();
      return;
    }

    this.proxyCanvas.width = image.width;
    this.proxyCanvas.height = image.height;

    if (image instanceof Image) {
      this.proxyContext.drawImage(image, 0, 0);
    } else {
      this.proxyContext.putImageData(image, 0, 0);
    }

    this.resizableCanvas.update();
  }

  render() {
    const { width, height } = this.props;

    return <ResizableCanvas width={width} height={height} onCanvasLoad={this._onCanvasLoad} />;
  }
}
