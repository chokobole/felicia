import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';

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
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { frame, topic } = viewState;

    if (frame) {
      const { width, height, frameRate, pixelFormat } = frame;
      return {
        width,
        height,
        frameRate,
        pixelFormat,
        topic,
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      topic,
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
