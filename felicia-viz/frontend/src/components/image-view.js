import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: "off" */
// import { ResizeDetector } from '@felicia-viz/ui';
import { CameraFrame } from 'store/camera';
import Worker from 'util/image-view-webworker.js';

const PROXY = 'proxy';
const MAIN = 'main';

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

  _onCanvasLoad(tag, ref) {
    if (!this._canvas) {
      this._canvas = {};
    }

    this._canvas[tag] = ref;

    if (!this._context) {
      this._context = {};
    }

    if (ref) {
      this._context[tag] = ref.getContext('2d');
    }
  }

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

    const { width, height, data, pixelFormat } = frame;

    this.worker.postMessage({
      source: 'imageView',
      data: {
        imageData: this._context[PROXY].getImageData(0, 0, width, height),
        width,
        height,
        data,
        pixelFormat,
      },
    });
  }

  _drawImageOrImageData(image) {
    if (!this._canvas) {
      return;
    }

    const { canvasWidth, canvasHeight } = this.props;

    const mainCanvas = this._canvas[MAIN];
    const mainContext = this._context[MAIN];
    const proxyCanvas = this._canvas[PROXY];
    const proxyContext = this._context[PROXY];

    if (image) {
      proxyCanvas.width = image.width;
      proxyCanvas.height = image.height;

      if (image instanceof Image) {
        proxyContext.drawImage(image, 0, 0);
      } else {
        proxyContext.putImageData(image, 0, 0);
      }

      mainCanvas.width = canvasWidth;
      let finalHeight = canvasHeight;
      if (canvasHeight === 'auto') {
        finalHeight = (canvasWidth / image.width) * image.height;
      }
      mainCanvas.height = finalHeight;
      // resize
      mainContext.drawImage(proxyCanvas, 0, 0, mainCanvas.width, mainCanvas.height);
    } else {
      mainContext.clearRect(0, 0, this.width, this.height);
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
        <canvas ref={this._onCanvasLoad.bind(this, PROXY)} style={{ display: 'none' }} />
        <canvas ref={this._onCanvasLoad.bind(this, MAIN)} />
      </div>
    );
  }
}
