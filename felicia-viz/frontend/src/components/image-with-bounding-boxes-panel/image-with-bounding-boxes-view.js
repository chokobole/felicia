import React, { Component } from 'react';
import PropTypes from 'prop-types';

import { ResizableCanvas } from '@felicia-viz/ui';

import { ImageWithBoundingBoxes } from 'store/ui/image-with-bounding-boxes-panel-state';
import Worker from 'util/image-with-bounding-boxes-view-webworker.js';

export default class ImageWithBoundingBoxesView extends Component {
  static propTypes = {
    width: PropTypes.string,
    height: PropTypes.string,
    frame: PropTypes.instanceOf(ImageWithBoundingBoxes),
    fontSize: PropTypes.number,
    lineWidth: PropTypes.number,
    threshold: PropTypes.number,
  };

  static defaultProps = {
    width: '100%',
    height: '100%',
    frame: null,
    fontSize: 15,
    lineWidth: 10,
    threshold: 0.5,
  };

  componentDidMount() {
    this.worker = new Worker();

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

    const { width, height } = frame.image;

    this.worker.postMessage({
      imageData: this.proxyContext.getImageData(0, 0, width, height),
      frame,
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

    const { imageData, boundingBoxes } = data;
    const { width, height } = imageData;
    const { lineWidth, fontSize, threshold } = this.props;

    this.proxyCanvas.width = width;
    this.proxyCanvas.height = height;

    this.proxyContext.putImageData(imageData, 0, 0);
    for (let i = 0; i < boundingBoxes.length; i += 1) {
      const { color, box, label, score } = boundingBoxes[i];
      if (score >= threshold) {
        const { leftTop, rightBottom } = box;
        const { r, g, b } = color;
        const x = leftTop.x * width;
        const y = leftTop.y * height;
        const w = (rightBottom.x - leftTop.x) * width;
        const h = (rightBottom.y - leftTop.y) * height;
        const colorStyle = `rgb(${Math.floor(r * 255)}, ${Math.floor(g * 255)}, ${Math.floor(
          b * 255
        )})`;
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
