import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import { DEPTH_CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';
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
    userHeader: { type: 'header', title: 'Camera Control' },
    sectionSeperator: { type: 'separator' },
    cameraInfo: {
      type: 'header',
      title: 'Info',
      children: {
        width: { type: 'custom', title: 'width', render: renderText },
        height: { type: 'custom', title: 'height', render: renderText },
        frameRate: { type: 'custom', title: 'frameRate', render: renderText },
        pixelFormat: { type: 'custom', title: 'pixelFormat', render: renderText },
        scale: { type: 'custom', title: 'scale', render: renderText },
      },
    },
    caemraControl: {
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
      },
    },
  };

  _onChange = values => {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();

    if (viewState.pointcloudView !== values.pointcloudView) {
      viewState.switchPointcloudView(values.pointcloudView);
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const { uiState } = store;
    const viewState = uiState.findView(uiState.activeViewState.id);
    const { frame, topic, filter, pointcloudView } = viewState;

    if (frame) {
      const { width, height, frameRate, pixelFormat, scale } = frame;
      return {
        width,
        height,
        frameRate,
        pixelFormat,
        scale,
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
