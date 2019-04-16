import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: "off" */
import { ResizeDetector } from '@felicia-viz/ui';
import { CameraFrame } from 'store/camera';
import Worker from 'util/webworker.js';

export default class ImageView extends PureComponent {
  static propTypes = {
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    src: PropTypes.string,
    frame: PropTypes.instanceOf(CameraFrame),
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    src: '',
    frame: null,
  };

  constructor(props) {
    super(props);

    this.state = {
      width: 0,
      height: 0,
      image: null,
      imageData: null,
    };

    this._context = null;
  }

  componentDidMount() {
    this.worker = new Worker();

    this.worker.onmessage = event => {
      this.setState({
        imageData: event.data,
      });
    };
  }

  componentWillUpdate(nextProps) {
    const { src, frame } = this.props;
    if (src !== nextProps.src) {
      this._loadImage();
    } else if (frame !== nextProps.frame) {
      this._loadImageData();
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

  _onResize = entry => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

  _loadImage() {
    const { src } = this.props;

    if (src) {
      const image = new Image();
      image.src = src;
      image.addEventListener(
        'load',
        () => {
          this.setState({
            image,
          });
        },
        false
      );
    }
  }

  _loadImageData() {
    if (!this._context) {
      return;
    }

    const { frame } = this.props;
    if (!frame) return;

    const { width, height } = frame;

    this.worker.postMessage({
      source: 'imageView',
      data: {
        imageData: this._context.getImageData(0, 0, width, height),
        frame,
      },
    });
  }

  _drawImageOrImageData() {
    if (!this._context) {
      return;
    }

    const { width, height, image, imageData } = this.state;
    if (!width) {
      return;
    }

    if (image) {
      this._canvas.width = width;
      let finalHeight = height;
      if (height === 'auto') {
        finalHeight = (width / image.width) * image.height;
      }
      this._canvas.height = finalHeight;
      this._context.drawImage(image, 0, 0, width, finalHeight);
    } else if (imageData) {
      this._canvas.width = imageData.width;
      this._canvas.height = imageData.height;
      this._context.putImageData(imageData, 0, 0);
    } else {
      this._context.clearRect(0, 0, width, height);
    }
  }

  render() {
    const { width, height } = this.props;
    const style = {
      position: 'relative',
      width,
      height,
    };

    this._drawImageOrImageData();

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}
