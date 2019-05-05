import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: "off" */
// import { ResizeDetector } from '@felicia-viz/ui';
import { CameraFrame } from 'store/camera';
import Worker from 'util/webworker.js';

export default class ImageView extends PureComponent {
  static propTypes = {
    canvasWidth: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    canvasHeight: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    src: PropTypes.string,
    frame: PropTypes.instanceOf(CameraFrame),
  };

  static defaultProps = {
    canvasWidth: '100%',
    canvasHeight: 'auto',
    src: '',
    frame: null,
  };

  constructor(props) {
    super(props);

    this._context = null;
  }

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      this._drawImageOrImageData(event.data);
    };
  }

  componentWillUpdate(nextProps) {
    const { src, frame } = this.props;

    if (src !== nextProps.src) {
      this._loadImage(nextProps.src);
    } else if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
    }
  }

  componentWillUnmount() {
    this.worker.terminate();
  }

  _onCanvasLoad = ref => {
    this._canvas = ref;

    if (ref) {
      this._context = ref.getContext('2d');
    }
  };

  // For later,
  // onResize = entry => {
  //  this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  // };

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
    if (!this._canvas) {
      return;
    }

    if (!frame) return;

    const { width, height, data } = frame;

    this.worker.postMessage({
      source: 'imageView',
      data: {
        imageData: this._context.getImageData(0, 0, width, height),
        width,
        height,
        data,
      },
    });
  }

  _drawImageOrImageData(image) {
    if (!this._canvas) {
      return;
    }

    const { canvasWidth, canvasHeight } = this.props;

    if (image) {
      const proxyCanvus = document.querySelector('#display-none-canvas');
      proxyCanvus.width = image.width;
      proxyCanvus.height = image.height;
      if (image instanceof Image) {
        proxyCanvus.getContext('2d').drawImage(image, 0, 0);
      } else {
        proxyCanvus.getContext('2d').putImageData(image, 0, 0);
      }

      this._canvas.width = canvasWidth;
      let finalHeight = canvasHeight;
      if (canvasHeight === 'auto') {
        finalHeight = (canvasWidth / image.width) * image.height;
      }
      this._canvas.height = finalHeight;
      // resize
      this._context.drawImage(proxyCanvus, 0, 0, this._canvas.width, this._canvas.height);
    } else {
      this._context.clearRect(0, 0, this.width, this.height);
    }
  }

  render() {
    // For later, <ResizeDetector onResize={this._onResize} />
    const { canvasWidth, canvasHeight, frame } = this.props;

    this.style = frame && {
      position: 'relative',
      width: canvasWidth,
      height: canvasHeight,
    };

    return (
      <div style={this.style}>
        <canvas id='display-none-canvas' style={{ display: 'none' }} ref={this._onCanvasLoad} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}
