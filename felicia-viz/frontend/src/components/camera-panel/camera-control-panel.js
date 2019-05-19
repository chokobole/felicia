import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import ColormapDropdown from './colormap-dropdown';

@inject('store')
@observer
export default class CameraControlPanel extends Component {
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
            return <TopicDropdown {...self} typeName={CAMERA_FRAME_MESSAGE} />;
          },
        },
        filter: {
          type: 'custom',
          title: 'filter',
          render: self => {
            return <ColormapDropdown {...self} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const { uiState } = store;
    const viewState = uiState.findView(uiState.activeViewState.id);
    const { frame, topic, filter } = viewState;

    if (frame) {
      const { width, height, frameRate, pixelFormat } = frame;
      return {
        width,
        height,
        frameRate,
        pixelFormat,
        topic,
        filter,
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      topic,
      filter,
    };
  }

  render() {
    return (
      <Form
        data={this.SETTINGS}
        values={this._fetchValues()}
        style={{
          wrapper: {
            padding: 12,
          },
        }}
        onChange={this._onChange}
      />
    );
  }
}
