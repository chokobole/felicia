import React, { PureComponent } from 'react';
import PropTypes from 'prop-types';

/* eslint import/no-unresolved: "off" */
import { ResizeDetector } from 'felicia-viz/ui';

export default class ImageSequence extends PureComponent {
  static propTypes = {
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    src: PropTypes.arrayOf(PropTypes.string),

    currentTime: PropTypes.number.isRequired
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
    src: []
  };

  constructor(props) {
    super(props);

    this.state = {
      width: 0,
      height: 0,
      currentFrameImage: null
    };

    this._loadFrame(0);

    this._context = null;
  }

  componentDidMount() {
    this._renderFrame();
  }

  componentWillUpdate(nextProps) {
    const { currentTime } = this.props;
    if (currentTime !== nextProps.currentTime) {
      this._loadFrame(nextProps.currentTime);
    }
  }

  _onCanvasLoad = (ref) => {
    this._canvas = ref;

    if (ref) {
      this._context = ref.getContext('2d');
    }
  };

  _onResize = (entry) => {
    this.setState({ width: entry.contentRect.width, height: entry.contentRect.height });
  };

  _loadFrame(time) {
    const imgName = `image${time}.jpg`;
    const { src } = this.props;
    const found = src.find(function(element) {
      return element.endsWith(imgName);
    });

    if (!found) return;

    const img = new Image();
    img.src = found;
    img.addEventListener(
      'load',
      () => {
        this.setState({
          currentFrameImage: img
        });
      },
      false
    );
  }

  _renderFrame() {
    if (!this._context) {
      return;
    }

    const { width, currentFrameImage } = this.state;
    let { height } = this.state;
    if (!width) {
      return;
    }

    if (!currentFrameImage) {
      this._context.clearRect(0, 0, width, height);
    } else {
      /* eslint react/destructuring-assignment: "off" */
      if (this.props.height === 'auto') {
        height = (width / currentFrameImage.width) * currentFrameImage.height;
      }
      this._canvas.width = width;
      this._canvas.height = height;
      this._context.drawImage(currentFrameImage, 0, 0, width, height);
    }
  }

  render() {
    const { width, height } = this.props;
    const style = {
      position: 'relative',
      width,
      height
    };

    this._renderFrame();

    return (
      <div style={style}>
        <ResizeDetector onResize={this._onResize} />
        <canvas ref={this._onCanvasLoad} />
      </div>
    );
  }
}
