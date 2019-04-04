import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: "off" */
import { ResizeDetector } from '@felicia-viz/ui';

export default class ImageView extends PureComponent {
  static propTypes = {
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    src: PropTypes.string,
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    src: '',
  };

  constructor(props) {
    super(props);

    this.state = {
      width: 0,
      height: 0,
      image: null,
    };

    this._loadImage();

    this._context = null;
  }

  componentWillUpdate(nextProps) {
    const { src } = this.props;
    if (src !== nextProps.src) {
      this._loadImage();
    }
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

    const img = new Image();
    img.src = src;
    img.addEventListener(
      'load',
      () => {
        this.setState({
          image: img,
        });
      },
      false
    );
  }

  _drawImage() {
    if (!this._context) {
      return;
    }

    const { width, image } = this.state;
    let { height } = this.state;
    if (!width) {
      return;
    }

    if (!image) {
      this._context.clearRect(0, 0, width, height);
    } else {
      /* eslint react/destructuring-assignment: "off" */
      if (this.props.height === 'auto') {
        height = (width / image.width) * image.height;
      }
      this._canvas.width = width;
      this._canvas.height = height;
      this._context.drawImage(image, 0, 0, width, height);
    }
  }

  render() {
    const { width, height } = this.props;
    const style = {
      position: 'relative',
      width,
      height,
    };

    this._drawImage();

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}
