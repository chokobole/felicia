import PropTypes from 'prop-types';
import React, { PureComponent } from 'react';

import ResizeDetector from './resize-detector';

const PROXY = 'proxy';
const MAIN = 'main';

export default class ResizableCanvas extends PureComponent {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    onCanvasLoad: PropTypes.func.isRequired,
  };

  static defaultProps = {
    width: '100%',
    height: 'auto',
  };

  constructor(props) {
    super(props);

    this._context = null;
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

    const { onCanvasLoad } = this.props;
    if (tag === PROXY) {
      onCanvasLoad(this._canvas[PROXY], this._context[PROXY], this);
    }
  }

  clearRect() {
    const mainCanvas = this._canvas[MAIN];
    const mainContext = this._context[MAIN];

    if (!(mainCanvas && mainContext)) return;

    const { width, height } = mainCanvas;

    mainContext.clearRect(0, 0, width, height);
  }

  update() {
    const mainCanvas = this._canvas[MAIN];
    const mainContext = this._context[MAIN];
    const proxyCanvas = this._canvas[PROXY];

    if (!(mainCanvas && mainContext && proxyCanvas)) return;

    const { width, height } = this.state;

    let finalHeight = height;
    // eslint-disable-next-line react/destructuring-assignment
    if (this.props.height === 'auto') {
      finalHeight = (width / proxyCanvas.width) * proxyCanvas.height;
    }

    mainCanvas.width = width;
    mainCanvas.height = height;
    mainContext.drawImage(proxyCanvas, 0, 0, width, finalHeight);
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
