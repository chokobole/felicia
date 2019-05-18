import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: ["error", { ignore: ['@felicia-viz/ui'] }] */
import { ResizeDetector } from '@felicia-viz/ui';
import { CameraFrame } from 'store/ui/camera-panel-state';
import Worker from 'util/image-view-webworker.js';

const PROXY = 'proxy';
const MAIN = 'main';

export default class ImageView extends PureComponent {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    src: PropTypes.string,
    frame: PropTypes.instanceOf(CameraFrame),
    filter: PropTypes.string,
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    src: '',
    frame: null,
    filter: '',
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

  _onResize = entry => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

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
    const { filter } = this.props;

    this.worker.postMessage({
      source: 'imageView',
      data: {
        imageData: this._context[PROXY].getImageData(0, 0, width, height),
        width,
        height,
        data,
        pixelFormat,
        filter,
      },
    });
  }

  _drawImageOrImageData(image) {
    if (!this._canvas) {
      return;
    }

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

      const { width, height } = this.state;

      mainCanvas.width = width;
      let finalHeight = height;
      // eslint-disable-next-line react/destructuring-assignment
      if (this.props.height === 'auto') {
        finalHeight = (width / image.width) * image.height;
      }
      mainCanvas.height = finalHeight;
      // resize
      mainContext.drawImage(proxyCanvas, 0, 0, mainCanvas.width, mainCanvas.height);
    } else {
      mainContext.clearRect(0, 0, this.width, this.height);
    }
  }

  render() {
    const { width, height } = this.props;

    const style = {
      width,
      height,
    };

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad.bind(this, PROXY)} style={{ display: 'none' }} />
        <canvas ref={this._onCanvasLoad.bind(this, MAIN)} />
      </div>
    );
  }
}
