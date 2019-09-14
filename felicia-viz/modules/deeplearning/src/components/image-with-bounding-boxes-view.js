import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';

import { ImageWithBoundingBoxesMessage } from '@felicia-viz/proto/messages/bounding-box';
import { ActivatableFloatPanel, ResizableCanvas } from '@felicia-viz/ui';
import ImageWorker from '@felicia-viz/ui/webworkers/image-webworker';

class ImageWithBoundingBoxesViewImpl extends Component {
  static propTypes = {
    width: PropTypes.string.isRequired,
    height: PropTypes.string.isRequired,
    frame: PropTypes.instanceOf(ImageWithBoundingBoxesMessage),
    fontSize: PropTypes.number,
    lineWidth: PropTypes.number,
    threshold: PropTypes.number,
  };

  static defaultProps = {
    frame: null,
    fontSize: 15,
    lineWidth: 10,
    threshold: 0.5,
  };

  componentDidMount() {
    this.worker = new ImageWorker();

    this.worker.onmessage = event => {
      this._drawImageDataAndBoundingBoxes(event.data);
    };
  }

  shouldComponentUpdate(nextProps) {
    const { frame, lineWidth, fontSize, threshold } = this.props;

    if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
      return true;
    }

    if (
      lineWidth !== nextProps.lineWidth ||
      fontSize !== nextProps.fontSize ||
      threshold !== nextProps.threshold
    ) {
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

  _loadImageData(frame) {
    if (!this.proxyContext) {
      return;
    }

    if (!frame) return;

    const { image, boundingBoxes } = frame;
    const { width, height } = image.size;

    this.worker.postMessage({
      imageData: this.proxyContext.getImageData(0, 0, width, height),
      image,
      data: {
        boundingBoxes,
      },
    });
  }

  _drawImageDataAndBoundingBoxes(data) {
    if (!this.proxyContext) {
      return;
    }

    if (!data) {
      this.resizableCanvas.clearRect();
      return;
    }

    const { imageData } = data;
    const { boundingBoxes } = data.data;
    const { width, height } = imageData;
    const { lineWidth, fontSize, threshold } = this.props;

    this.proxyCanvas.width = width;
    this.proxyCanvas.height = height;

    this.proxyContext.putImageData(imageData, 0, 0);
    for (let i = 0; i < boundingBoxes.length; i += 1) {
      const { color, box, label, score } = boundingBoxes[i];
      if (score >= threshold) {
        const { r, g, b } = color;
        const { topLeft, bottomRight } = box;
        const { x, y } = topLeft;
        const w = bottomRight.x - x;
        const h = bottomRight.y - y;
        const colorStyle = `rgb(${r}, ${g}, ${b})`;
        this.proxyContext.beginPath();
        this.proxyContext.font = `${fontSize}px sans`;
        this.proxyContext.fillStyle = colorStyle;
        this.proxyContext.strokeStyle = colorStyle;
        this.proxyContext.lineWidth = lineWidth;
        this.proxyContext.fillText(label, x, y);
        this.proxyContext.rect(x + lineWidth / 2, y + lineWidth / 2, w - lineWidth, h - lineWidth);
        this.proxyContext.stroke();
      }
    }

    this.resizableCanvas.update();
  }

  render() {
    const { width, height } = this.props;

    return <ResizableCanvas width={width} height={height} onCanvasLoad={this._onCanvasLoad} />;
  }
}

@inject('store')
@observer
export default class ImageWithBoundingBoxesView extends Component {
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
    const { frame, lineWidth, threshold } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='ImageWithBoundingBoxesView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <ImageWithBoundingBoxesViewImpl
          width={width}
          height={height}
          frame={frame}
          lineWidth={lineWidth}
          threshold={threshold}
        />
      </ActivatableFloatPanel>
    );
  }
}
