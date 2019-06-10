import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import {
  CAMERA_FRAME_MESSAGE,
  DEPTH_CAMERA_FRAME_MESSAGE,
  PixelFormat,
} from '@felicia-viz/communication';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';
import ColormapDropdown from './colormap-dropdown';

@inject('store')
@observer
export default class DepthCameraControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'Camera Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        width: { type: 'custom', title: 'width', render: renderText },
        height: { type: 'custom', title: 'height', render: renderText },
        frameRate: { type: 'custom', title: 'frameRate', render: renderText },
        pixelFormat: { type: 'custom', title: 'pixelFormat', render: renderText },
        scale: { type: 'custom', title: 'scale', render: renderText },
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
        pointcloudView: {
          type: 'toggle',
          title: 'pointcloudView',
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

  _onChange = values => {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();

    if (viewState.pointcloudView !== values.pointcloudView) {
      viewState.setPointcloudView(values.pointcloudView);
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { frame, topic, filter, pointcloudView } = viewState;

    if (frame) {
      const { width, height, frameRate, pixelFormat, scale, timestamp } = frame;
      return {
        width,
        height,
        frameRate,
        pixelFormat: PixelFormat.valuesById[pixelFormat],
        scale,
        timestamp,
        topic,
        filter,
        pointcloudView,
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      scale: '',
      timestamp: '',
      topic,
      filter,
      pointcloudView,
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
