import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form, Label } from '@streetscape.gl/monochrome';

import PanelItemContainer from 'components/panel-item-container';
import ColormapDropdown from 'components/colormap-dropdown';
import { CAMERA_FRAME_MESSAGE } from 'common/felicia-proto';
import TopicDropdown from 'components/topic-dropdown';
import { FeliciaVizStore } from 'store';

function renderText(self) {
  const { title, value } = self;
  return (
    <PanelItemContainer>
      <Label>{title}</Label>
      <Label>{value}</Label>
    </PanelItemContainer>
  );
}

@inject('store')
@observer
export default class CameraControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
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
    const cameraPanelState = uiState.findCameraPanel(uiState.activeWindow.id);
    const { camera, topic, filter } = cameraPanelState;
    const { frame } = camera;

    return {
      width: frame ? frame.width : '',
      height: frame ? frame.height : '',
      frameRate: frame ? frame.frameRate : '',
      pixelFormat: frame ? frame.pixelFormat : '',
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
