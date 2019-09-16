import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import {
  POSE3F_WITH_TIMESTAMP_MESSAGE,
  POSE3D_WITH_TIMESTAMP_MESSAGE,
  Pose3WithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';

import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import TopicDropdown from './common/topic-dropdown';
import { renderText } from './common/panel-item';

@inject('store')
@observer
export default class Pose3WithTimestampControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'Pose3WithTimestamp Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        position: { type: 'custom', title: 'position', render: renderText },
        orientation: { type: 'custom', title: 'orientation', render: renderText },
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
            return (
              <TopicDropdown
                {...self}
                typeNames={[POSE3F_WITH_TIMESTAMP_MESSAGE, POSE3D_WITH_TIMESTAMP_MESSAGE]}
              />
            );
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
    const { pose } = viewState;

    if (pose && pose instanceof Pose3WithTimestampMessage) {
      const { position, orientation, timestamp } = pose;
      return {
        position: position.toShortString(),
        orientation: orientation.toShortString(),
        timestamp,
      };
    }
    return {
      position: '',
      orientation: '',
      timestamp: '',
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
