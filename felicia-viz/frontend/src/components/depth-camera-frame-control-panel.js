import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { PixelFormat } from '@felicia-viz/proto/messages/ui';
import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/camera-frame-message';
import { DEPTH_CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';
import ColormapDropdown from './colormap-dropdown';

@inject('store')
@observer
export default class DepthCameraFrameControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'DepthCameraFrame Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        width: { type: 'custom', title: 'width', render: renderText },
        height: { type: 'custom', title: 'height', render: renderText },
        frameRate: { type: 'custom', title: 'frameRate', render: renderText },
        pixelFormat: { type: 'custom', title: 'pixelFormat', render: renderText },
        min: { type: 'custom', title: 'min', render: renderText },
        max: { type: 'custom', title: 'max', render: renderText },
        timestamp: { type: 'custom', title: 'timestamp', render: renderText },
      },
    },
    control: {
      type: 'header',
      title: 'Control',
      children: {
        topic: {
          type: 'custom',
          title: 'topic',
          render: self => {
            return <TopicDropdown {...self} typeName={DEPTH_CAMERA_FRAME_MESSAGE} />;
          },
        },
        filter: {
          type: 'custom',
          title: 'filter',
          render: self => {
            return <ColormapDropdown {...self} />;
          },
        },
        topicToAlign: {
          type: 'custom',
          title: 'topicToAlign',
          render: self => {
            return <TopicDropdown {...self} typeName={CAMERA_FRAME_MESSAGE} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { frame, filter } = viewState;

    if (frame) {
      const { cameraFormat, min, max, timestamp } = frame;
      const { size, pixelFormat, frameRate } = cameraFormat;
      const { width, height } = size;
      return {
        width,
        height,
        frameRate,
        pixelFormat: PixelFormat.valuesById[pixelFormat],
        min,
        max,
        timestamp,
        filter,
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      min: '',
      max: '',
      timestamp: '',
      filter,
    };
  }

  render() {
    return (
      <Form
        data={this.SETTINGS}
        values={this._fetchValues()}
        style={FORM_STYLE}
        onChange={this._onChange}
      />
    );
  }
}
